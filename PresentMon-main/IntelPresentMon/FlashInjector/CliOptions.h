#pragma once
#include "../CommonUtilities/cli/CliFramework.h"
#include "../CommonUtilities/log/Level.h"

namespace clio
{
	using namespace pmon::util::cli;
	using namespace pmon::util::log;
	using namespace std::literals;
	struct Options : public OptionsBase<Options>
	{
		Option<std::string> exeName{ this, "--exe-name", "", "Name of the .exe module to target processes of" };
		Option<float> barSize{ this, "--bar-size", .25f, "Size of the flashing bar" };
		Option<float> barRightShift{ this, "--bar-right-shift", .5f, "Distance to offset the bar to the right" };
		Option<std::string> barColor{ this, "--bar-color", "255,255,255"s, "Color of the bar (e.g. 255,255,255 for white)." };
		Option<std::string> backgroundColor{ this, "--background-color", "0,0,0"s, "Color of the background (e.g. 255,255,255 for white)." };
		Flag renderBackground{ this, "--render-background", "Always display a black background for the bar" };
		Option<uint32_t> pollPeriod{ this, "--poll-period", 100, "Period at which to poll for new target processes (ms)" };
		Flag waitForUserInput{ this, "--wait-for-user-input", "Display a pop-up to wait for user input" };
		// Option<std::string> logFile{ this, "--log-file", "", "Path to file to output log entries to" };

		static constexpr const char* description = "Helper utility for drawing a rectangular flash within a target process via injection";
		static constexpr const char* name = "FlashInjector.exe";
	};
}