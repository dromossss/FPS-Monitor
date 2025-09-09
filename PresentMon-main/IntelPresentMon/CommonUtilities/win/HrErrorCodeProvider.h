#pragma once
#include "../log/IErrorCodeProvider.h"
#include <cstdint>

namespace pmon::util::win
{
	struct hr_wrap
	{
		hr_wrap(uint32_t ucode) : pmlog_code{ (int32_t)ucode } {}
		int32_t pmlog_code;
	};

	class HrErrorCodeProvider : public log::IErrorCodeProvider
	{
	public:
		std::type_index GetTargetType() const override;
		log::IErrorCodeResolver::Strings Resolve(const log::ErrorCode&) const override;
	};
}