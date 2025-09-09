#include "PanicLogger.h"
#include <fstream>
#include <iostream>
#include <array>
#include <filesystem>
#include "../win/WinAPI.h"

namespace pmon::util::log
{
	class PanicLogger_
	{
	public:
		PanicLogger_() noexcept
		{
			try {
				file_.open("pmlog-panic.txt", file_.out|file_.app);
			}
			catch (...) {
				LogLastChance_("Could not open pmlog-panic.txt in working directory, reverting to temp directory");
			}
			if (!file_) {
				try {
					file_.open(std::filesystem::temp_directory_path() / "pmlog-panic.txt", file_.out | file_.app);
				}
				catch (...) {
					LogLastChance_("Could not open pmlog-panic.txt in temp directory, file panic logging will not be used");
				}
			}
		}
		void Log(const std::string& msg) noexcept
		{
			LogWinDbg_(msg);
			LogStdErr_(msg);
			LogFile_(msg);
		}
	private:
		// functions
		void LogStdErr_(const std::string& msg) const noexcept
		{
			try {
				std::cerr << "[@Error|log_panic] " << msg << std::endl;
			}
			catch (...) {
				LogLastChance_("Failed panic logging to stderr");
			}
		}
		void LogWinDbg_(const std::string& msg) const noexcept
		{
			OutputDebugStringA("[@Error|log_panic] ");
			OutputDebugStringA(msg.c_str());
			OutputDebugStringA("\n");
		}
		void LogFile_(const std::string& msg) noexcept
		{
			if (file_) {
				try {
					file_ << "[@Error|log_panic] " << msg << std::endl;
					if (!file_) {
						LogLastChance_("Error occurred when panic logging to file");
					}
				}
				catch (...) {
					LogLastChance_("Failed panic logging to file");
				}
			}
		}
		void LogLastChance_(const std::string& msg) const noexcept
		{
			LogWinDbg_(msg);
		}
		static std::array<char, 250> GetTimestamp_() noexcept;
		// data
		std::ofstream file_;
	};

	void PMLogPanic_(const std::string& msg) noexcept
	{
		PanicLogger_{}.Log(msg);
	}
}