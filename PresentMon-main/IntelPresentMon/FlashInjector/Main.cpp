#include "../CommonUtilities/win/WinAPI.h"
#include "../CommonUtilities/str/String.h"
#include "CliOptions.h"

#include <set>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <format>

#include "LibraryInject.h"
#include "Logging.h"

namespace stdfs = std::filesystem;
using namespace pmon::util;
using namespace std::literals;

// null logger factory to satisfy linking requirements for CommonUtilities
namespace pmon::util::log
{
    std::shared_ptr<class IChannel> GetDefaultChannel() noexcept
    {
        return {};
    }
}

int main (int argc, char **argv)
{
    // Initialize arguments
    if (auto res = clio::Options::Init(argc, argv)) {
        return *res;
    }
    auto& opts = clio::Options::Get();

    auto executableName = *opts.exeName;

    stdfs::path dxgiOverlayPath;
    {
        std::vector<char> buffer(MAX_PATH);
        auto size = GetModuleFileNameA(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (size == 0) {
            LOGE << "Failed to get DXGIOverlay executable path.";
        }
        dxgiOverlayPath = std::string(buffer.begin(), buffer.begin() + size);
        dxgiOverlayPath = dxgiOverlayPath.parent_path();
    }
 
    // DLL to inject
    const stdfs::path libraryPath = dxgiOverlayPath / std::format("FlashInjectorLibrary-{}.dll", PM_BUILD_PLATFORM);

    // Configuration file
    auto cfgFilePath = stdfs::temp_directory_path()/"GfxLayer.cfg";
    LOGI << "    Writing configuration to " << cfgFilePath;

    std::ofstream cfgFile(cfgFilePath);
    //if (opts.logFile) {
    //    auto logFile = stdfs::absolute(*opts.logFile);
    //    stdfs::remove(logFile);

    //    auto opt = "LogFile=" + logFile.string();
    //    LOGI << "        " << opt;
    //    cfgFile << opt << std::endl;
    //}
    if (opts.waitForUserInput) {
        auto opt = "WaitForUserInput=1";
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    if (opts.barSize) {
        auto opt = "BarSize=" + std::to_string(*opts.barSize);
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    if (opts.barRightShift)
    {
        auto opt = "BarRightShift=" + std::to_string(*opts.barRightShift);
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    if (opts.barColor)
    {
        auto opt = "BarColor=" + *opts.barColor;
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    if (opts.backgroundColor)
    {
        auto opt = "BackgroundColor=" + *opts.backgroundColor;
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    if (opts.renderBackground)
    {
        auto opt = "RenderBackground=1";
        LOGI << "        " << opt;
        cfgFile << opt << std::endl;
    }
    cfgFile.close();

    LOGI << "";
    LOGI << "Inject options: ";
    LOGI << "    Executable name: " << executableName;
    LOGI << "    DLL to inject:  " << libraryPath;
    LOGI << "";

    if (!stdfs::exists(libraryPath)) {
        LOGE << "Cannot find library: " << libraryPath;
        exit(1);
    }

    LOGI << "Waiting for processes that match executable name...";

    std::unordered_set<DWORD> processesAttached;
    while (true) {
        auto processes = LibraryInject::GetProcessNames();
        auto executableNameLower = str::ToLower(executableName);

        for (auto&& [processId, processName] : processes) {
            const auto processNameLower = str::ToLower(processName);
            if (processNameLower == executableNameLower && !processesAttached.contains(processId)) {
                LOGI << "    Injecting DLL to process with PID: " << processId;
                LibraryInject::Attach(processId, libraryPath);
                processesAttached.insert(processId);
            }
        }
        // check for new matching processes every n milliseconds
        std::this_thread::sleep_for(*opts.pollPeriod * 1ms);
    }

    return 0;
}