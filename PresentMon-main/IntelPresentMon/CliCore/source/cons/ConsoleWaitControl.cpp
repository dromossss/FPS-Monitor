#include "ConsoleWaitControl.h"
#include <Core/source/win/WinAPI.h>
#include <Core/source/win/com/ProcessSpawnSink.h>
#include <Core/source/win/com/WbemConnection.h>
#include <Core/source/win/com/WbemListener.h>
#include <filesystem>
#include <ranges>
#include <set>
#include <array>
#include "../opt/Options.h"


namespace p2c::cli::cons
{
	using namespace std::chrono_literals;
	namespace rn = std::ranges;
	namespace vi = rn::views;

	class ProcessTracker : private win::com::ProcessSpawnSink::EventQueue
	{
	public:
		ProcessTracker(std::function<void()> notify)
			:
			win::com::ProcessSpawnSink::EventQueue{ [this] { HandleEvents_(); } },
			notifyFunction_{ std::move(notify) },
			pSpawnListener_{ conn_.MakeListener<win::com::ProcessSpawnSink>(
				static_cast<win::com::ProcessSpawnSink::EventQueue&>(*this)) }
		{}
		void AddWatch(std::string watch)
		{
			std::lock_guard lk{ mtx_ };
			watchedNames_.insert(std::move(watch));
		}
		void RemoveWatch(std::string watch)
		{
			std::lock_guard lk{ mtx_ };
			watchedNames_.erase(watch);
		}
		std::optional<win::Process> GetMatchEvent()
		{
			std::optional<win::Process> match;
			std::lock_guard lk{ mtx_ };
			if (!matches_.empty()) {
				match = std::move(matches_.back());
				matches_.pop_back();
			}
			return match;
		}
		void SimulateSpawnEvent(p2c::win::Process process)
		{
			if (HandleOneEvent_(std::move(process))) {
				notifyFunction_();
			}
		}
	private:
		// functions
		void HandleEvents_()
		{
			bool matchOccurred = false;
			while (auto e = Pop()) {
				if (HandleOneEvent_(std::move(*e))) {
					matchOccurred = true;
				}
			}
			if (matchOccurred) {
				notifyFunction_();
			}
		}
		bool HandleOneEvent_(p2c::win::Process e)
		{
			bool matchOccurred = false;
			auto moduleName = std::filesystem::path{ e.name }.filename().string();
			if (opt::get().ignoreCase) {
				moduleName = moduleName
					| vi::transform([](char c) {return(char)std::tolower(c); })
					| rn::to<std::basic_string>();
			}
			std::lock_guard lk{ mtx_ };
			if (auto i = watchedNames_.find(moduleName); i != watchedNames_.end()) {
				matches_.push_back(std::move(e));
				watchedNames_.erase(i);
				matchOccurred = true;
			}
			return matchOccurred;
		}
		// data
		win::com::WbemConnection conn_;
		std::mutex mtx_;
		std::set<std::string> watchedNames_;
		std::vector<win::Process> matches_;
		std::function<void()> notifyFunction_;
		std::unique_ptr<win::com::WbemListener> pSpawnListener_;
	};

	class TargetDeathTracker
	{
		class Tracker
		{
		public:
			Tracker(TargetDeathTracker* pParent, uint32_t pid)
				:
				pParent_{ pParent },
				processHandle_{ OpenProcess(SYNCHRONIZE, false, pid) },
				wakeEventHandle_{ CreateEventW(nullptr, true, false, nullptr) },
				thread_{ [this] {ThreadProc_(); } }
			{}
			~Tracker()
			{
				SetEvent(wakeEventHandle_);
				thread_.join();
				CloseHandle(processHandle_);
				CloseHandle(wakeEventHandle_);
			}
			bool Died() const
			{
				return died_;
			}
            Tracker(const Tracker&) = delete;
            Tracker& operator=(const Tracker&) = delete;
		private:
			// functions
			void ThreadProc_()
			{
				const std::array handles{ processHandle_, wakeEventHandle_ };
				const auto wakeReason = WaitForMultipleObjects(
					(DWORD)handles.size(), handles.data(), FALSE, INFINITE
				);
				if (wakeReason == WAIT_OBJECT_0) {
					died_ = true;
					pParent_->SignalDeathEvent_();
				}
			}
			// data
			TargetDeathTracker* pParent_;
			HANDLE processHandle_;
			HANDLE wakeEventHandle_;
			std::atomic<bool> died_ = false;
			std::thread thread_;
		};
	public:
		TargetDeathTracker(std::function<void()> notify)
			:
			notifyFunction_{ std::move(notify) }
		{}
		void Add(uint32_t pid)
		{
			std::lock_guard lk{ mtx_ };
			trackers_.emplace(pid,
				std::make_unique<Tracker>(this, pid)
			);
		}
		void Remove(uint32_t pid)
		{
			std::lock_guard lk{ mtx_ };
			trackers_.erase(pid);
		}
		std::optional<uint32_t> GetDeathEvent()
		{
			std::optional<uint32_t> deathPid;
			std::lock_guard lk{ mtx_ };
			if (auto i = rn::find_if(trackers_, [](const auto& t)
				{ return t.second->Died(); }); i != trackers_.end()) {
				deathPid = i->first;
				trackers_.erase(i);
			}
			return deathPid;
		}
	private:
		// functions
		void SignalDeathEvent_()
		{
			std::lock_guard lk{ mtx_ };
			notifyFunction_();
		}
		// data
		std::mutex mtx_;
		std::unordered_map<uint32_t, std::unique_ptr<Tracker>> trackers_;
		std::function<void()> notifyFunction_;
	};



	ConsoleWaitControl::ConsoleWaitControl()
		:
		pProcessTracker_{ std::make_unique<ProcessTracker>([this] { SignalSpawn(); }) },
		pTargetDeathTracker_{ std::make_unique<TargetDeathTracker>([this] { SignalDeath(); }) }
	{
		// TODO: check return and log/throw
		SetConsoleCtrlHandler(ConsoleWaitControl::CtrlHandler, TRUE);
	}
	ConsoleWaitControl::~ConsoleWaitControl()
	{
		SetConsoleCtrlHandler(ConsoleWaitControl::CtrlHandler, FALSE);
	}
	ConsoleWaitControl::WakeReason ConsoleWaitControl::WaitFor(std::chrono::milliseconds ms)
	{
		std::unique_lock lk{ mtx_ };
		cv_.wait_for(lk, ms, [this] { return EventPending(); });
		return ExtractWakeReason();
	}
	ConsoleWaitControl::WakeReason ConsoleWaitControl::Wait()
	{
		std::unique_lock lk{ mtx_ };
		cv_.wait(lk, [this] { return EventPending(); });
		return ExtractWakeReason();
	}
	void ConsoleWaitControl::AddProcessSpawnWatch(std::string processName)
	{
		pProcessTracker_->AddWatch(std::move(processName));
	}
	void ConsoleWaitControl::RemoveProcessSpawnWatch(const std::string& processName)
	{
		pProcessTracker_->RemoveWatch(processName);
	}
	void ConsoleWaitControl::SetTimer(std::chrono::milliseconds ms)
	{
		if (!timerThread_.joinable()) {
			timerThread_ = std::jthread{ &ConsoleWaitControl::TimerKernel, this, ms };
		}
		else {
			throw std::logic_error{ "Trying to set console wait control timer twice" };
		}
	}
	std::optional<win::Process> ConsoleWaitControl::GetSpawnEvent()
	{
		std::unique_lock lk{ mtx_ };
		return pProcessTracker_->GetMatchEvent();
	}
	void ConsoleWaitControl::AddProcessDeathWatch(uint32_t pid)
	{
		std::unique_lock lk{ mtx_ };
		pTargetDeathTracker_->Add(pid);
	}
	std::optional<uint32_t> ConsoleWaitControl::GetProcessDeathEvent()
	{
		std::unique_lock lk{ mtx_ };
		return pTargetDeathTracker_->GetDeathEvent();
	}
	void ConsoleWaitControl::Exit()
	{
		exitSignal_.release();
	}
	ConsoleWaitControl& ConsoleWaitControl::Get()
	{
		static ConsoleWaitControl singleton;
		return singleton;
	}
	void ConsoleWaitControl::SimulateSpawnEvent(p2c::win::Process process)
	{
		pProcessTracker_->SimulateSpawnEvent(std::move(process));
	}
	void ConsoleWaitControl::NotifyEndOfLogEvent()
	{
		{
			std::lock_guard lk{ mtx_ };
			endOfLOgEventPending_ = true;
		}
		cv_.notify_all();
	}
	int __stdcall ConsoleWaitControl::CtrlHandler(unsigned long type)
	{
		return Get().SignalCtrl(type);
	}
	int ConsoleWaitControl::SignalCtrl(unsigned long type)
	{
		{
			std::lock_guard lk{ mtx_ };
			dying_ = true;
		}
		// unblock main thread with termination reason
		cv_.notify_all();
		// wait 1s for ack from main thread
		(void)exitSignal_.try_acquire_for(1s);
		// returning here will kill process, either after main thread ack or 1s timeout
		return TRUE;
	}
	void ConsoleWaitControl::SignalSpawn()
	{
		{
			std::lock_guard lk{ mtx_ };
			processSpawnEventPending_ = true;
		}
		// unblock main thread with process spawn reason
		cv_.notify_all();
	}

	void ConsoleWaitControl::SignalDeath()
	{
		{
			std::lock_guard lk{ mtx_ };
			processDeathEventPending_ = true;
		}
		// unblock main thread with process spawn reason
		cv_.notify_all();
	}

	void ConsoleWaitControl::TimerKernel(std::chrono::milliseconds ms)
	{
		auto st = timerThread_.get_stop_token();
		std::condition_variable_any cv;
		std::unique_lock lk{ mtx_ };
		cv.wait_for(lk, st, ms, [] { return false; });
		if (!st.stop_requested()) {
			timerEventPending_ = true;
			lk.unlock();
			cv_.notify_all();
		}
	}
	ConsoleWaitControl::WakeReason ConsoleWaitControl::ExtractWakeReason()
	{
		if (dying_) {
			return WakeReason::CtrlTermination;
		}
		else if (endOfLOgEventPending_) {
			return WakeReason::EndOfLog;
		}
		else if (processSpawnEventPending_) {
			processSpawnEventPending_ = false;
			return WakeReason::ProcessSpawn;
		}
		else if (timerEventPending_) {
			timerThread_.join();
			timerEventPending_ = false;
			return WakeReason::TimerEvent;
		}
		else if (processDeathEventPending_) {
			processDeathEventPending_ = false;
			return WakeReason::ProcessDeath;
		}
		else {
			return WakeReason::Timeout;
		}
	}
	bool ConsoleWaitControl::EventPending() const
	{
		return dying_ || processSpawnEventPending_ || timerEventPending_ ||
			processDeathEventPending_ || endOfLOgEventPending_;
	}
}

