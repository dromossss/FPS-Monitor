#pragma once

#include <unordered_map>
#include <filesystem>
#include <cstdint>
#include <string>

using ProcessMap = std::unordered_map<uint32_t, std::string>;

namespace LibraryInject
{
	ProcessMap GetProcessNames();
	void Attach(uint32_t processId, const std::filesystem::path& dllPath);
}