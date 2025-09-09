#pragma once
#include "WinAPI.h"
#include "../Exception.h"

namespace pmon::util::win
{
	PM_DEFINE_EX(WinError);

	class HrError : public WinError {
	public:
		HrError(HRESULT code);
		HrError(HRESULT code, std::string msg);
		HrError(std::string msg);
		HrError();
		std::string ComposeWhatString_() const noexcept override;
		HRESULT GetCode() const noexcept;
		PM_STATUS GeneratePmStatus() const noexcept override;
		bool HasPmStatus() const noexcept override;
	private:
		HRESULT code_;
	};
}