#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <semaphore>
#include <optional>
#include <Core/source/win/Process.h>

namespace p2c::cli::cons
{
	class ProcessTracker;
	class TargetDeathTracker;

	class ConsoleWaitControl
	{
		friend ProcessTracker;
		friend TargetDeathTracker;
	public:
		// types
		enum class WakeReason
		{
			Timeout,
			ProcessSpawn,
			CtrlTermination,
			TimerEvent,
			ProcessDeath,
			EndOfLog,
		};
		// functions
		ConsoleWaitControl(const ConsoleWaitControl&) = delete;
		ConsoleWaitControl& operator=(const ConsoleWaitControl&) = delete;
		~ConsoleWaitControl();
		WakeReason WaitFor(std::chrono::milliseconds ms);
		WakeReason Wait();
		void AddProcessSpawnWatch(std::string processName);
		void RemoveProcessSpawnWatch(const std::string& processName);
		void SetTimer(std::chrono::milliseconds ms);
		std::optional<win::Process> GetSpawnEvent();
		void AddProcessDeathWatch(uint32_t pid);
		std::optional<uint32_t> GetProcessDeathEvent();
		void Exit();
		static ConsoleWaitControl& Get();
		void SimulateSpawnEvent(p2c::win::Process process);
		void NotifyEndOfLogEvent();
		bool EventPending() const;
	private:
		// functions
		ConsoleWaitControl();
		// TODO: consider race condition where main thread exists normally
		// just as ctrl command triggers handler (delayed wait to exit)
		static int __stdcall CtrlHandler(unsigned long type);
		int SignalCtrl(unsigned long type);
		void SignalSpawn();
		void SignalDeath();
		void TimerKernel(std::chrono::milliseconds ms);
		WakeReason ExtractWakeReason();
		// data
		std::condition_variable cv_;
		std::mutex mtx_;
		bool dying_ = false;
		bool processSpawnEventPending_ = false;
		bool timerEventPending_ = false;
		bool processDeathEventPending_ = false;
		bool endOfLOgEventPending_ = false;
		std::binary_semaphore exitSignal_{ 0 };
		std::unique_ptr<ProcessTracker> pProcessTracker_;
		std::unique_ptr<TargetDeathTracker> pTargetDeathTracker_;
		std::jthread timerThread_;
	};
}