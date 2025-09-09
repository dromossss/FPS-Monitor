#include "Level.h"
#include "../str/String.h"

namespace pmon::util::log
{
	std::string GetLevelName(Level lv) noexcept
	{
		try {
			switch (lv) {
			case Level::None: return "None";
			case Level::Fatal: return "Fatal";
			case Level::Error: return "Error";
			case Level::Warning: return "Warning";
			case Level::Info: return "Info";
			case Level::Performance: return "Performance";
			case Level::Debug: return "Debug";
			case Level::Verbose: return "Verbose";
			default: return "Unknown";
			}
		} catch (...) {}
		return {};
	}

	std::map<std::string, Level> GetLevelMapNarrow() noexcept
	{
		using namespace pmon::util::str;
		std::map<std::string, Level> map;
		for (int n = (int)Level::Fatal; n <= (int)Level::Verbose; n++) {
			const auto lvl = Level(n);
			auto key = ToLower(GetLevelName(lvl));
			if (key != "unknown") {
				map[std::move(key)] = lvl;
			}
		}
		return map;
	}
}