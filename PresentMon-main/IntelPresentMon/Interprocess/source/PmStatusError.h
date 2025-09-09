#pragma once
#include "../../CommonUtilities/Exception.h"
#include "../../PresentMonAPI2/PresentMonAPI.h"
#include <sstream>

namespace pmon::ipc
{
	class PmStatusError : public util::Exception {
	public:
		PmStatusError(PM_STATUS code);
		PmStatusError(PM_STATUS code, std::string msg);
		std::string ComposeWhatString_() const noexcept override;
		PM_STATUS GetCode() const noexcept;
		PM_STATUS GeneratePmStatus() const noexcept override;
		bool HasPmStatus() const noexcept override;
	private:
		PM_STATUS code_;
	};
}