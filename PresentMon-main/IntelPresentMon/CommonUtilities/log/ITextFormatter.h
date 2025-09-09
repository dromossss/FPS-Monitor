#pragma once
#include <string>

namespace pmon::util::log
{
	struct Entry;

	class ITextFormatter
	{
	public:
		virtual ~ITextFormatter() = default;
		virtual std::string Format(const Entry&) const = 0;
	};
}
