#pragma once
#include <optional>
#include <string>

namespace p2c::cli::svc
{
	void Boot(bool logging, std::optional<std::string> logPath, std::optional<int> logLevel);
}