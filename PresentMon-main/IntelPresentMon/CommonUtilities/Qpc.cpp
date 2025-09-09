#include "Qpc.h"
#include "win/WinAPI.h"
#include "log/Log.h"
#include <thread>


namespace pmon::util
{
	int64_t GetCurrentTimestamp() noexcept
	{
		LARGE_INTEGER timestamp;
		if (!QueryPerformanceCounter(&timestamp)) {
			pmlog_error("qpc failed").hr().every(50);
		}
		return (int64_t)timestamp.QuadPart;
	}
	double GetTimestampPeriodSeconds() noexcept
	{
		LARGE_INTEGER freq;
		if (!QueryPerformanceFrequency(&freq)) {
			pmlog_error("qpc frequency failed").hr().every(5);
		}
		return 1.0 / double(freq.QuadPart);
	}
	void SpinWaitUntilTimestamp(int64_t timestamp) noexcept
	{
		while (GetCurrentTimestamp() < timestamp) {
			std::this_thread::yield();
		}
	}
	double TimestampDeltaToSeconds(int64_t start, int64_t end, double period) noexcept
	{
		return double(end - start) * period;
	}


	QpcTimer::QpcTimer() noexcept
	{
		performanceCounterPeriod_ = GetTimestampPeriodSeconds();
		Mark();
	}
	double QpcTimer::Mark() noexcept
	{
		const auto newTimestamp = GetCurrentTimestamp();
		const auto delta = TimestampDeltaToSeconds(startTimestamp_, newTimestamp, performanceCounterPeriod_);
		startTimestamp_ = newTimestamp;
		return delta;
	}
	double QpcTimer::Peek() const noexcept
	{
		const auto newTimestamp = GetCurrentTimestamp();
		const auto delta = TimestampDeltaToSeconds(startTimestamp_, newTimestamp, performanceCounterPeriod_);
		return delta;
	}
	int64_t QpcTimer::GetStartTimestamp() const noexcept
	{
		return startTimestamp_;
	}
	void QpcTimer::SpinWaitUntil(double seconds) const noexcept
	{
		while (Peek() < seconds) {
			std::this_thread::yield();
		}
	}
}