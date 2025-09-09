#pragma once
#include <Memory>
#include <filesystem>
#include "../CommonUtilities/log/IChannel.h"
#include "../PresentMonAPI2/PresentMonDiagnostics.h"

namespace pmon::util::log
{
	void SetupCopyChannel(std::shared_ptr<IChannel> pCopyTargetChannel) noexcept;
	void SetupODSChannel(Level logLevel, Level stackTraceLevel, bool exceptionTrace) noexcept;
	void SetupDiagnosticChannel(const PM_DIAGNOSTIC_CONFIGURATION* pConfig) noexcept;
	void SetupFileChannel(std::filesystem::path path, Level logLevel, Level stackTraceLevel,
		bool exceptionTrace) noexcept;
	std::shared_ptr<class DiagnosticDriver> GetDiagnostics();
}