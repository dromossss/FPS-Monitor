#pragma once
#include <typeindex>
#include <variant>
#include <cstdint>
#include <string>

namespace pmon::util::log
{
	class ErrorCode;
}

namespace pmon::util::log
{
	class IErrorCodeResolver
	{
	public:
		// types
		struct Strings
		{
			std::string type;
			std::string symbol;
			std::string name;
			std::string description;
		};
		// functions
		virtual ~IErrorCodeResolver() = default;
		virtual Strings Resolve(std::type_index typeIdx, const ErrorCode&) const = 0;
	};
}