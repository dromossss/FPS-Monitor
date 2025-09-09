#include "HrLogger.h"
#include "../win/WinAPI.h"
#include "../Exception.h"

namespace pmon::util::log
{
	HrLogger::HrLogger(const char* file, const char* func, int line) noexcept
		:
		sourceFile_{ file },
		sourceFunctionName_{ func },
		sourceLine_{ line }
	{}
	void HrLogger::operator<<(uint32_t hr) const
	{
		if (FAILED(hr)) {
			if ((PMLOG_BUILD_LEVEL_ >= Level::Error) || (GlobalPolicy::Get().GetLogLevel() >= Level::Error)) {
				EntryBuilder{ Level::Error, sourceFile_, sourceFunctionName_, sourceLine_ }.hr(hr);
			}
			throw Except<Exception>("failed hr check");
		}
	}
}