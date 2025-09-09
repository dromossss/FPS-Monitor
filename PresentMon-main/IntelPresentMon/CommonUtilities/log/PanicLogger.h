#pragma once
#include <string>
#include <optional>

#ifndef NDEBUG
#define pmlog_panic_ PMLogPanic_
#else
#define pmlog_panic_
#endif

namespace pmon::util::log
{
	void PMLogPanic_(const std::string& msg) noexcept;
	template<class C>
	void PMLogPanic_(const std::pair<std::string, std::optional<C>>& msg) noexcept
	{
		PMLogPanic_(msg.first);
	}
}

