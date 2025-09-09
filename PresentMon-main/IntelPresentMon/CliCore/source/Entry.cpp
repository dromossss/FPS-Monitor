#include <Core/source/infra/log/Logging.h>
#include <Core/source/infra/util/Util.h>
#include <CLI/CLI.hpp>
#include <format>
#include <cstdint>
#include <iostream>
#include "pmon/FrameDataStream.h"
#include "dat/CsvWriter.h"
#include "dat/FrameFilterPump.h"
#include "dat/FrameDemultiplexer.h"
#include <stdexcept>
#include "opt/Options.h"
#include "cons/ConsoleWaitControl.h"
#include <Core/source/infra/util/ChiliTimer.h>
#include <map>
#include <set>
#include <ranges>
#include <cassert>
#include <Core/source/win/ProcessMapBuilder.h>
#include "svc/Boot.h"
#include "cons/OptionalConsoleOut.h"
#include "dat/MakeCsvName.h"
#include "pmon/Client.h"
#include "pmon/stream/LoggedFrameDataState.h"


namespace p2c::cli
{
	using infra::util::ToWide;
	using infra::util::ToNarrow;
	using namespace std::chrono_literals;
	using namespace std::string_literals;
	namespace rn = std::ranges;
	namespace vi = rn::views;

	int Entry(int argc, char** argv)
	{	
		// keep track of how long since cli app launch for the
		// purposes of start/stop timing
		infra::util::ChiliTimer launchTimer;

		// parse command line options
		try {
			opt::init(argc, argv);
		}
		catch (const CLI::ParseError& e) {
			return e.get_exit_code();
		};

		// shortcut for command line options
		const auto& opts = opt::get();

		// boot services
		svc::Boot(
			opts.log,
			opts.logPath ? std::optional{ *opts.logPath } : std::nullopt,
			opts.logLevel ? std::optional{ *opts.logLevel } : std::nullopt
		);

		try {
			using WakeReason = cons::ConsoleWaitControl::WakeReason;
			auto& waiter = cons::ConsoleWaitControl::Get();

			std::shared_ptr<pmon::Client> pClient;
			// stream used to make sure etl frames get processed when there are no other streams
			// this is necessary because when processes are specified by name, we need to pull frames
			// to generate the spawn events, but spawn events are only generated when there is a stream
			// and a stream will only be created if there is a spawn event (chicken and egg problem)
			const auto telemetryPeriod = opts.telemetryPeriod ? *opts.telemetryPeriod : 16;
			std::shared_ptr<pmon::FrameDataStream> pLogSpoolingStream;
			if (opts.etlFile) {
				pClient = std::make_shared<pmon::LoggedClient>(*opts.etlFile, waiter, telemetryPeriod);
				pLogSpoolingStream = pClient->OpenStream(0);
			}
			else {
				pClient = std::make_shared<pmon::LiveClient>(telemetryPeriod);
			}

			// list adapters
			if (opts.listAdapters) {
				const auto adapters = pClient->EnumerateAdapters();
				if (adapters.empty()) {
					std::cout << "No adapters compatible with PresentMon GPU telemetry available." << std::endl;
				}
				else {
					std::cout << "Available adapters:" << std::endl;
				}
				for (auto& a : adapters) {
					std::cout << " " << a.id << ": [" << a.vendor << "] " << a.name << std::endl;
				}
				return 0;
			}

			cons::OptionalConsoleOut out{ !opts.outputStdout };

			// set adapter for gpu telemetry
			if (opts.adapter) {
				const auto adapters = pClient->EnumerateAdapters();
				if (const auto i = rn::find_if(adapters, [&opts](const auto& a)
					{return a.id == *opts.adapter; }); i == adapters.end()) {
					std::cout << "Adapter with ID [" << *opts.adapter << "] not found." << std::endl;
					return -1;
				}
				else {
					out << "Using for GPU telemetry adapter <" << i->id << ">: [" << i->vendor << "] " << i->name << std::endl;
				}
				pClient->SetAdapter(*opts.adapter);
			}

			// count of the number of specifically-targetted processes (by name and pid)
			const auto targetCount = (*opts.name).size() + (*opts.pid).size();
			// derived flag for whether we are capturing all processes
			const auto captureAll = opts.captureAll || targetCount == 0;
			// targets specified by name that have been detected and have had a stream created for them
			std::set<std::wstring> mappedProcessNames;
			// frame pumps for each stream, which also own the sinks for their outgoing frame data
			std::vector<std::shared_ptr<dat::FrameFilterPump>> pumps;
			// count total number of processes being monitored for the purposes of exiting
			int trackedProcessCount = 0;
			// count number of monitored processes that have exited
			int exitedProcessCount = 0;
			// output file name option as std::optional
			const auto outputFileName = opts.outputFile ? std::optional{ *opts.outputFile } : std::nullopt;
			// setup csv column groups
			auto csvGroups = opts.GetColumnGroups();
			// setup excludes
			auto excludes = *opts.excludeName;
			if (!opts.ignoreCase) {
				for (auto& e : excludes) {
					e = e | vi::transform([](char c) {return(char)std::tolower(c); })
						  | rn::to<std::basic_string>();
				}
			}
			// function decides how streams should be added based on 2x2 matrix
			// of options: single/multi-csv x omni/targeted-streams
			const auto AddStream = [&](uint32_t pid, std::optional<std::string> processName = {}) {
				// non-omni stream usage cases:
				// if we are targetting w/ multi-csv, add stream + writer
				// if we are targetting single process (regardless of csv-mode), same
				if (!captureAll && (opts.multiCsv || targetCount == 1)) {
					auto name = dat::MakeCsvName(opts.noCsv, pid, std::move(processName), outputFileName);
					auto pWriter = std::make_shared<dat::CsvWriter>(std::move(name), csvGroups, opts.outputStdout);
					pumps.push_back(std::make_shared<dat::FrameFilterPump>(
						pClient->OpenStream(pid), std::move(pWriter),
						excludes, std::vector<uint32_t>{}, opts.excludeDropped, opts.ignoreCase
					));
					waiter.AddProcessDeathWatch(pid);
					++trackedProcessCount;
				}
				// omni stream usage cases:
				// if we are doing omni capture x multi-csv, add omni stream w/ demux
				else if (captureAll && opts.multiCsv) {
					assert(pumps.empty());
					auto pDemux = std::make_shared<dat::FrameDemultiplexer>(csvGroups, outputFileName);
					pumps.push_back(std::make_shared<dat::FrameFilterPump>(
						pClient->OpenStream(0), std::move(pDemux),
						excludes, std::vector<uint32_t>{}, opts.excludeDropped, opts.ignoreCase
					));
				}
				// if we are doing omni capture x single-csv, add omni w/ writer
				else if (captureAll && !opts.multiCsv) {
					assert(pumps.empty());
					auto name = dat::MakeCsvName(opts.noCsv, {}, {}, outputFileName);
					auto pWriter = std::make_shared<dat::CsvWriter>(std::move(name), csvGroups, opts.outputStdout);
					pumps.push_back(std::make_shared<dat::FrameFilterPump>(
						pClient->OpenStream(0), std::move(pWriter),
						excludes, std::vector<uint32_t>{}, opts.excludeDropped, opts.ignoreCase
					));
				}
				// if we are doing multi-target capture x single-csv, add omni w/ whitelist
				// if one doesn't exist, or add whitelist to existing otherwise
				else {
					if (pumps.empty()) {
						auto name = dat::MakeCsvName(opts.noCsv, {}, {}, outputFileName);
						auto pWriter = std::make_shared<dat::CsvWriter>(std::move(name), csvGroups, opts.outputStdout);
						pumps.push_back(std::make_shared<dat::FrameFilterPump>(
							pClient->OpenStream(0), std::move(pWriter), excludes,
							std::vector<uint32_t>{ pid }, opts.excludeDropped, opts.ignoreCase
						));
					}
					else {
						pumps.front()->AddInclude(pid);
					}
					waiter.AddProcessDeathWatch(pid);
					++trackedProcessCount;
				}
			};

			// delay before starting any captures
			if (opts.startAfter) {
				// subtract time elapsed since launch from the desired start time to get wait time
				const auto delaySeconds = std::max(0., *opts.startAfter - launchTimer.Peek());
				out << std::format("Waiting {:.3f}s for start delay.", *opts.startAfter) << std::endl;
				const auto delayMs = 1ms * int(1000.f * delaySeconds);
				waiter.SetTimer(delayMs);
				if (waiter.Wait() == WakeReason::CtrlTermination) {
					out << "Termination signal received during wait. Exiting..." << std::endl;
					waiter.Exit();
					return 0;
				}
			}

			// immediately start streams for all processes specified by pid
			if (opts.pid) {
				for (auto pid : *opts.pid) {
					out << "Capturing process with PID [" << pid << "]." << std::endl;
					AddStream(pid);
				}
			}

			// try to start streams for all processes specified by name
			// maintain a watch on all named processes not yet running so that a stream can be
			// started as soon as a matching process spawns
			if (opts.name) {
				for (auto name : *opts.name) {
					if (opts.ignoreCase) {
						name = name
							| vi::transform([](char c) {return(char)std::tolower(c); })
							| rn::to<std::basic_string>();
					}
					// begin watching for process spawn before we check for pre-existence
					// this guards against race conditions of spawn after check but before watch
					waiter.AddProcessSpawnWatch(name);

					// now check for pre-existence and open stream if available (only for live captures)
					if (!opts.etlFile) {
						auto procMap = win::ProcessMapBuilder{}.AsNameMap(opts.ignoreCase);
						if (const auto i = procMap.find(ToWide(name)); i != procMap.end()) {
							// we found that our target process is already active!
							const auto pid = i->second.pid;
							out << "Capturing process [" << name << "] with PID [" << pid << "]." << std::endl;
							AddStream(pid, name);
							// we found by polling, so remove from the watch
							waiter.RemoveProcessSpawnWatch(ToNarrow(i->first));
							// also add to set of opened process names so that we can ignore any
							// spawn events if they were already emitted
							mappedProcessNames.insert(i->first);
						}
						else {
							// target process is not yet open
							out << "Waiting for [" << name << "] to spawn." << std::endl;
						}
					}
				}
			}

			// start omni-stream capture
			if (captureAll || (!opts.pid && !opts.name)) {
				out << "Capturing all processes." << std::endl;
				AddStream(0);
			}

			// set stop timer
			if (opts.captureFor) {
				out << std::format("Setting capture timer to {:.3f}s.", *opts.captureFor) << std::endl;
				const auto delayMs = 1ms * int(1000.f * *opts.captureFor);
				waiter.SetTimer(delayMs);
			}

			// periodically run FrameFilterPump Process() function for all active streams
			// which reads in frame data and writes it to corresponding csv file(s)
			// also respond to spawn events by adding new streams
			// also respond to ctrl events by doing cleanup and shutting down
			// also respond to timer events by doing cleanup and shutting down
			// also respond to process death events by shutting down when all dead
			// poll at high rate for offline etl processing to protect again race condition (early close)
			const auto livePollPeriod = opts.pollPeriod ? 1ms * *opts.pollPeriod : 250ms;
			const auto pollingPeriod = opts.etlFile ? 1ms : livePollPeriod;
			// simulated timestamp used for pulling frames from streams
			double timestamp = 0.;
			for (WakeReason wakeReason = WakeReason::Timeout;;
				wakeReason = waiter.WaitFor(pollingPeriod)) {
				switch (wakeReason) {
				case WakeReason::CtrlTermination:
					out << "Received termination control signal. Exiting...." << std::endl;
					// Exit() call unblocks the signal handler thread
					waiter.Exit();
					return 0;
				case WakeReason::EndOfLog:
					out << "Reached end of ETW log. Exiting...." << std::endl;
					waiter.Exit();
					return 0;
				case WakeReason::TimerEvent:
					out << "Requested stop time reached. Exiting...." << std::endl;
					// Exit() call protects again race condition where if ctrl signal
					// handler is call right after timer wake, we make sure it doesn't block
					waiter.Exit();
					return 0;
				case WakeReason::ProcessDeath:
					while (auto p = waiter.GetProcessDeathEvent()) {
						// non-omni target mode, remove process from stream set
						if (!captureAll && (opts.multiCsv || targetCount == 1)) {
							std::erase_if(pumps, [&](const auto& s) {return s->GetPid() == *p; });
						}
						// keep track of how many processes have exited
						out << "Detected that process with pid [" << *p << "] has exited." << std::endl;
						exitedProcessCount++;
					}
					// if last target process dies, we will exit
					if (exitedProcessCount >= trackedProcessCount &&
						exitedProcessCount >= targetCount) {
						out << "All target processes have exited. Exiting...." << std::endl;
						waiter.Exit();
						return 0;
					}
					break;
				case WakeReason::ProcessSpawn:
					while (auto p = waiter.GetSpawnEvent()) {
						// ignore processes already opened
						if (!mappedProcessNames.contains(p->name)) {
							// add process to set of names to ignore
							mappedProcessNames.insert(p->name);
							// open stream
							AddStream(p->pid, ToNarrow(p->name));
							out << "Detected process [" << ToNarrow(p->name) << "] with pid [" << p->pid << "]. Starting capture...." << std::endl;
						}
					}
					break;
				case WakeReason::Timeout:
					// use this stream when processing etl file to make sure that frames are pulled
					// and process spawn events are generated
					if (pLogSpoolingStream) {
						pLogSpoolingStream->Pull(timestamp);
					}
					// pump streams if there are no events pending
					if (!waiter.EventPending()) {
						// process all pumps if there was a polling timeout
						for (auto& pump : pumps) {
							pump->Process(timestamp);
						}
						// update timestamp so cache is refreshed
						timestamp++;
					}
					break;
				default:
					assert(false && "Unknown WakeReason encountered in main stream processing loop");
				}
			}
		}
		catch (const std::exception& e) {
			std::cout << "Error: [" << e.what() << "].\n";
			std::cout << "Use --help to see a list of options supported by this program." << std::endl;
		}

		return -1;
	}
}