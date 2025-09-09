#pragma once
#include <typeindex>
#include "IErrorCodeResolver.h"

namespace pmon::util::log
{
	class IErrorCodeProvider
	{
	public:
		virtual ~IErrorCodeProvider() = default;
		virtual std::type_index GetTargetType() const = 0;
		virtual IErrorCodeResolver::Strings Resolve(const ErrorCode&) const = 0;
	};
}