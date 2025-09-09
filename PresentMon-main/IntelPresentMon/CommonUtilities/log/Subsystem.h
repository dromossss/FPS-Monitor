#pragma once
#include <string>
#include <map>
#include <cstdint>

namespace pmon::util::log
{
	enum class Subsystem : uint16_t
	{
		None,
		Middleware,
		Server,
		Wrapper,
		IntelPresentmon,
		User = 0x8000,
	};

	std::string GetSubsystemName(Subsystem) noexcept;
}
