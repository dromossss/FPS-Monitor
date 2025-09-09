#include "../CommonUtilities/win/MessageBox.h"
#include "../CommonUtilities/win/Utilities.h"
#include <memory>
#include <string>
#include <format>

#include "Context.h"
#include "Hooks/Hooks.h"
#include "Custom/Extensions.h"
#include "../FlashInjector/Logging.h"

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

namespace GfxLayer
{
    void WaitForUserInput()
    {
        auto message = "GfxLayer.dll has been injected to the following application: \n"s + win::GetExecutableModulePath().string() + "\n\n"s;
        message += "Press OK to continue running the application."s;
        win::MsgBox{ std::move(message) }.WithTitle("Pausing Target Application").AsModal();
    }

    void Initialize()
    {
        LOGI << "GfxLayer attached!";

        // Parse the config file
        ConfigParser cfgParser;
        cfgParser.Parse();

        auto& opt = cfgParser.GetOptions();
        LOGI << "Loaded options from: " << cfgParser.GetConfigFile();
        opt.Print();

        Context::GetInstance().SetOptions(opt);

        if (opt.GetFlag(GFXL_OPT_WAIT_FOR_USER_INPUT))
        {
            WaitForUserInput();
        }

        Extension::Initialize();

        // Hook APIs
        Hooks::DXGI::Hook_DXGI();
        Hooks::D3D10::Hook_D3D10();
        Hooks::D3D11::Hook_D3D11();
    }
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID pReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            GfxLayer::Initialize();
            break;

        case DLL_PROCESS_DETACH:
            if (pReserved == nullptr)
            {}
            break;
    }

    return TRUE;
}