#pragma once
#include <chrono>

namespace pmon::util::log
{
	struct TimePoint
	{
		TimePoint() noexcept;
		std::chrono::high_resolution_clock::time_point value;
	};
}