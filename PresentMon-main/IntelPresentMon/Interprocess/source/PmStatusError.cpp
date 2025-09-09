#include "PmStatusError.h"
#include <sstream>

namespace pmon::ipc
{
	PmStatusError::PmStatusError(PM_STATUS code) :
		Exception{ "PresentMon error in service or middleware" },
		code_{ code } {}

	PmStatusError::PmStatusError(PM_STATUS code, std::string msg) :
		Exception{ std::move(msg) },
		code_{ code } {}

	std::string PmStatusError::ComposeWhatString_() const noexcept
	{
		try {
			std::ostringstream oss;
			oss << GetNote() << " CODE:[" << code_ << "]";
			if (HasTrace()) {
				oss << "\n" << GetTraceString();
			}
			return oss.str();
		}
		catch (...) {}
		return {};
	}

	PM_STATUS PmStatusError::GetCode() const noexcept { return code_; }

	PM_STATUS PmStatusError::GeneratePmStatus() const noexcept
	{
		return GetCode();
	}

	bool PmStatusError::HasPmStatus() const noexcept
	{
		return true;
	}
}