#pragma once
#include "ITextFormatter.h"
#include <vector>
#include <memory>

namespace pmon::util::log
{
	struct Entry;
	class IErrorCodeResolver;

	class TextFormatter : public ITextFormatter
	{
	public:
		std::string Format(const Entry&) const override;
	};
}
