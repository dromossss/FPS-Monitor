#pragma once
#include <cstdint>
#include "Log.h"

namespace pmon::util::log
{
	class HrLogger
	{
	public:
		HrLogger(const char*, const char*, int) noexcept;
		void operator<<(uint32_t hr) const;
	private:
		const char* sourceFile_ = nullptr;
		const char* sourceFunctionName_ = nullptr;
		int sourceLine_ = 0;
	};
}

#define pmlog_hr ::pmon::util::log::HrLogger{ __FILE__, __FUNCTION__, __LINE__ }