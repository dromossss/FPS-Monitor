#include "Subsystem.h"
#include "../str/String.h"

namespace pmon::util::log
{
	std::string GetSubsystemName(Subsystem lv) noexcept
	{
		try {
			switch (lv) {
			case Subsystem::None: return "None";
			case Subsystem::Middleware: return "Middleware";
			case Subsystem::Server: return "Server";
			case Subsystem::Wrapper: return "Wrapper";
			case Subsystem::IntelPresentmon: return "IntelPresentmon";
			default:
				if (int(lv) < int(Subsystem::User)) {
					return "Unknown";
				}
				else {
					return "User" + std::to_string(int(lv) - int(Subsystem::User));
				}
			}
		}
		catch (...) {}
		return {};
	}
}