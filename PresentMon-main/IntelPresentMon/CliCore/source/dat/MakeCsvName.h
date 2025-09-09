#pragma once
#include <string>
#include <optional>

namespace p2c::cli::dat
{
	std::string MakeCsvName(bool noCsv, std::optional<uint32_t> pid, std::optional<std::string> processName, std::optional<std::string> customName);
}