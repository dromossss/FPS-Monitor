#include "IntervalWaiter.h"
#include <thread>

namespace pmon::util
{
	IntervalWaiter::IntervalWaiter(double intervalSeconds, double waitBuffer)
		:
		intervalSeconds_{ intervalSeconds },
		waiter_{ waitBuffer }
	{}

	void IntervalWaiter::SetInterval(double intervalSeconds)
	{
		intervalSeconds_ = intervalSeconds;
	}

	void IntervalWaiter::SetInterval(std::chrono::nanoseconds interval)
	{
		intervalSeconds_ = double(interval.count()) / 1'000'000'000.;
	}

	void IntervalWaiter::Wait()
	{
		const auto waitTimeSeconds = [=, this] {
			const auto t = timer_.Peek();
			auto targetCandidate = lastTargetTime_ + intervalSeconds_;
			// if we are on-time
			if (t <= targetCandidate) {
				lastTargetTime_ = targetCandidate;
				return targetCandidate - t;
			}
			// if we are late, reset target to NOW and do not wait
			lastTargetTime_ = t;
			return 0.;
		}();
		if (waitTimeSeconds > 0.) {
			waiter_.Wait(waitTimeSeconds);
		}
	}
}
