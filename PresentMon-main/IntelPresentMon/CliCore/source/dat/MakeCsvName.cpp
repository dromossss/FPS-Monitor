#include "MakeCsvName.h"
#include <filesystem>
#include <sstream>

using namespace std::string_literals;

namespace p2c::cli::dat
{
	std::string MakeCsvName(bool noCsv, std::optional<uint32_t> pid, std::optional<std::string> processName, std::optional<std::string> customName)
	{
		if (noCsv) {
			return {};
		}
		std::string customPrefix;
		std::string customExtension;
		if (customName) {
			std::filesystem::path customPath{ *customName };
			if (customPath.has_parent_path()) {
				customPrefix += customPath.parent_path().string() + "\\";
			}
			if (customPath.has_stem()) {
				customPrefix += customPath.stem().string();
			}
			else {
				customPrefix += "PresentMon";
			}
			if (customPath.has_extension()) {
				customExtension = customPath.extension().string();
			}
			else {
				customExtension = ".csv"s;
			}
		}
		const std::chrono::zoned_time timestamp{
			std::chrono::current_zone(),
			std::chrono::system_clock::now()
		};
		std::stringstream buf;
		if (customName) {
			buf << customPrefix;
		}
		else {
			buf << "PresentMon";
		}
		if (processName) {
			buf << "-" << *processName;
		}
		if (processName && pid) {
			buf << std::format("[{}]", *pid);
		}
		else if (pid) {
			buf << std::format("-[{}]", *pid);
		}
		buf << std::format("-{0:%y}{0:%m}{0:%d}-{0:%H}{0:%M}{0:%OS}", timestamp);
		if (customName) {
			buf << customExtension;
		}
		else {
			buf << ".csv";
		}
		return buf.str();
	}
}