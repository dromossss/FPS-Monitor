#pragma once
#include "Framework.h"
#include <string>
#include <vector>
#include <CliCore/source/dat/ColumnGroups.h>

namespace p2c::cli::opt
{
	namespace impl
	{
		struct OptionsStruct : public OptionStructBase_
		{
			// add options and switches here to augment the CLI
		private: Group gGeneric{ "Options" }; public:
			Flag log{ "-l, --log", "Enable logging (off by default)" };
			Option<int> logLevel{ "--log_level", "Level to log at from 4 (most logging) to 0 (least logging, default)", [this](CLI::Option* p) {
				p->needs(log.opt()); } };
			Option<std::string> logPath{ "--log_path", "Directory to write logs to (defaults to working dir)", [this](CLI::Option* p) {
				p->needs(log.opt()); } };

		private: Group gTarget{ "Capture Target Options" }; public:
			Flag captureAll{ "-A,--captureall", "Capture all processes presenting on system. Default behavior." };
			Option<std::vector<std::string>> name{ "--process_name", "Name(s) of process(es) to capture" };
			Option<std::vector<std::string>> excludeName{ "--exclude", "Name(s) of process(es) to exclude", [this](CLI::Option* p) {
				p->excludes(name.opt()); } };
			Option<std::vector<int>> pid{ "--process_id", "ID(s) of process(es) to capture", [this](CLI::Option* p) {
				p->check(CLI::PositiveNumber)->excludes(captureAll.opt(), name.opt(), excludeName.opt()); }};
			Option<std::string> etlFile{ "--etl_file", "Source frame data from an externally-captured ETL file instead of live ETW events" };
			Flag ignoreCase{ "-i,--ignore_case", "Ignore case when matching a process name" };

		private: Group gOutput{ "Output Options" }; public:
			Option<std::string> outputFile{ "-o,--output_file", "Path, root name, and extension used for CSV file(s)" };
			Flag outputStdout{ "-s,--output_stdout", "Write frame data to stdout instead of status messages" };
			Flag multiCsv{ "-m,--multi_csv", "Write frame data from each process to a separate CSV file", [this](CLI::Option* p) {
				p->excludes(outputStdout.opt()); } };
			Flag noCsv{ "-v,--no_csv", "Disable CSV file output", [this](CLI::Option* p) {
				p->needs(outputStdout.opt()); } };

		private: Group gRecording{ "Recording Options" }; public:
			Option<double> startAfter{ "--delay", "Time in seconds from launch to start of capture" };
			Option<double> captureFor{ "--timed", "Time in seconds from start of capture to exit" };
			Flag excludeDropped{ "-d,--exclude_dropped", "Exclude dropped frames from capture" };

		private: Group gColumns{ "CSV Column Options" }; public:
#define X_(name, desc) Flag name{ "--" #name, desc };
			COLUMN_GROUP_LIST
#undef X_

		private: Group gAdapters{ "Graphics Adapter Options" }; public:
			Flag listAdapters{ "-a,--list_adapters", "List adaptors available for selection as telemetry source" };
			Option<int> adapter{ "--adapter", "Index of adapter to use as telemetry source", [this](CLI::Option* p) {
				p->check(CLI::NonNegativeNumber); } };

		private: Group gHidden{ "" }; public:
			Option<int> telemetryPeriod{ "--telemetry_period", "Period that the service uses to poll hardware telemetry", [this](CLI::Option* p) {
				p->check(CLI::PositiveNumber); } };
			Option<int> pollPeriod{ "--poll_period", "Period that this client uses to poll the service for frame data", [this](CLI::Option* p) {
				p->check(CLI::PositiveNumber); } };

			// used for getting vector of enabled CSV column groups
			std::vector<std::string> GetColumnGroups() const;

		protected:
			// edit application name and description here
			std::string GetName() const override
			{ return "PresentMonCli"; }
			std::string GetDescription() const override
			{ return "Command line interface for capturing frame presentation data using the PresentMon service."; }
		};
	}

	inline void init(int argc, char** argv) { impl::App<impl::OptionsStruct>::Init(argc, argv); }
	inline auto& get() { return impl::App<impl::OptionsStruct>::GetOptions(); }
}

#ifndef PMCLI_OPTS_CPP
#undef COLUMN_GROUP_LIST
#endif