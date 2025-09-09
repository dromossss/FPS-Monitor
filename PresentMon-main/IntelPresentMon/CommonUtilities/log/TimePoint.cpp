#include "TimePoint.h"
#include "Log.h"

namespace pmon::util::log
{
	TimePoint::TimePoint() noexcept
	{
		if constexpr (PMLOG_BUILD_LEVEL_ >= Level::Performance) {
			value = std::chrono::high_resolution_clock::now();
		}
		else {
			value = {};
		}
	}
}