#include "Config.h"
#include "../FlashInjector/Logging.h"
#include "../CommonUtilities/win/WinAPI.h"

#include <filesystem>
#include <fstream>
#include <ranges>

namespace GfxLayer
{
	static std::string Trim(const std::string& s) 
	{
		auto isSpace = [](char c) { return std::isspace<char>(c, std::locale::classic()); };
		auto begin   = std::find_if_not(s.begin(), s.end(), isSpace);
		auto end     = std::find_if_not(s.rbegin(), s.rend(), isSpace).base();

		// Empty string
		if (begin >= end)
		{
			return "";
		}

		return std::string(begin, end);
	}

	static bool IsSet(const std::unordered_map<std::string, std::string>& map, const std::string& key)
	{
		if (map.count(key))
		{
			return map.at(key).compare("1");
		}
		return false;
	}

	void Options::Add(const std::string& name, const std::string& value)
	{
		m_Map[name] = value;
	}

	bool Options::GetFlag(const std::string& name) const
	{
		if (m_Map.count(name))
		{
			return (m_Map.at(name).compare("1") == 0);
		}
		return false;
	}

	std::string Options::GetString(const std::string& name) const
	{
		if (m_Map.count(name))
		{
			return m_Map.at(name);
		}
		return "";
	}

	float Options::GetFloat(const std::string& name) const
	{
		if (m_Map.count(name))
		{
			return (float)std::atof(m_Map.at(name).c_str());
		}
		return std::numeric_limits<float>::quiet_NaN();
	}

	std::optional<Color> Options::GetRgb24Color(const std::string& name) const
	{
		try {
			if (m_Map.contains(name)) {
				const auto components = m_Map.at(name) | std::views::split(',') |
					std::views::transform([](auto&& subrange) {
					auto val = std::ranges::to<std::basic_string>(subrange);
					return std::stol(val);
				}) | std::ranges::to<std::vector>();
				if (components.size() == 3) {
					Color c;
					c.r = float(components[0]) / 255.f;
					c.g = float(components[1]) / 255.f;
					c.b = float(components[2]) / 255.f;
					c.a = 1.f;
					return c;
				}
			}
		}
		catch (...) {}
		return std::nullopt;
	}

	void Options::Print() const
	{
		LOGI << "GfxLayer Options:";
		for (auto& kv : m_Map)
		{
			LOGI << "    " << kv.first << " = " << kv.second;
		}
	}

	ConfigParser::ConfigParser():
		m_ConfigFile(""),
		m_Options({})
	{
		auto logFileName = "GfxLayerLog_" + std::to_string(GetCurrentProcessId()) + ".txt";
		auto cfgFileName = "GfxLayer.cfg";
		auto currentPath = std::filesystem::current_path();
		auto globalPath = std::filesystem::temp_directory_path();

		LOGI << "Default configuration file search paths:";
		LOGI << "    " << currentPath;
		LOGI << "    " << globalPath;

		m_ConfigFile = (std::filesystem::temp_directory_path()/cfgFileName).string();
		if (std::filesystem::exists(currentPath/cfgFileName))
		{
			m_ConfigFile = (currentPath/cfgFileName).string();
		}
		
		// Set default options
		m_Options.Add(GFXL_OPT_LOG_FILE, (currentPath/logFileName).string());
		m_Options.Add(GFXL_OPT_OUTPUT_DIR, currentPath.string());
		m_Options.Add(GFXL_OPT_ENABLE_DEBUG_LAYER, "0");
		m_Options.Add(GFXL_OPT_WAIT_FOR_USER_INPUT, "0");
	}

	ConfigParser::~ConfigParser()
	{}

	std::string ConfigParser::GetConfigFile()
	{
		return m_ConfigFile;
	}

	void ConfigParser::SetConfigFile(const std::string& configFile)
	{
		m_ConfigFile = configFile;
	}

	void ConfigParser::Parse()
	{
		auto configFile = std::filesystem::path(m_ConfigFile);

		// Open configuration file

		std::ifstream inputFileStream(configFile, std::ios::in);
		if (!inputFileStream)
		{
			LOGI << "Can't find configuration file: " << m_ConfigFile;
			return;
		}

		// Load all the "value=key" pairs

		std::string line;
		while (std::getline(inputFileStream, line))
		{
			std::istringstream iStrLine(line);
			std::string key;
			if (std::getline(iStrLine, key, '='))
			{
				std::string value;
				if (std::getline(iStrLine, value))
				{
					key = Trim(key);
					value = Trim(value);
					m_Options.Add(key, value);
				}
			}
		}
		inputFileStream.close();
	}

	const Options& ConfigParser::GetOptions() const
	{
		return m_Options;
	}
}
