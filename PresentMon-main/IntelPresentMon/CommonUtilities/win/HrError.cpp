#include "HrError.h"
#include "Utilities.h"
#include <sstream>

namespace pmon::util::win
{
	HrError::HrError(HRESULT code) :
		WinError{ "Windows error" },
		code_{ code } {}

	HrError::HrError(HRESULT code, std::string msg) :
		WinError{ std::move(msg) },
		code_{ code } {}

	HrError::HrError(std::string msg) :
		WinError{ std::move(msg) },
		code_{ (HRESULT)GetLastError() } {}

	HrError::HrError() :
		WinError{ "Windows error" },
		code_{ (HRESULT)GetLastError() } {}

	std::string HrError::ComposeWhatString_() const noexcept
	{
		try {
			std::ostringstream oss;
			oss << GetNote() << " HRESULT:[" << std::hex << code_ << std::dec << "] => "
				<< win::GetErrorDescription(code_);
			if (HasTrace()) {
				oss << "\n" << GetTraceString();
			}
			return oss.str();
		}
		catch (...) {}
		return {};
	}

	HRESULT HrError::GetCode() const noexcept { return code_; }

	PM_STATUS HrError::GeneratePmStatus() const noexcept
	{
		return PM_STATUS_FAILURE;
	}

	bool HrError::HasPmStatus() const noexcept
	{
		return false;
	}
}