#include "../Hooks/Hooks.h"
#include "../../CommonUtilities/win/WinAPI.h"
#include "../Logging.h"

#include <detours/detours.h>

namespace GfxLayer::Hooks
{
    static void Initialize()
    {
        static bool s_Initialized = false;

        if (!s_Initialized)
        {
            s_Initialized = true;
            DetourRestoreAfterWith();
        }
    }

    bool HookAPI(const std::string& name, PVOID* ppRealFn, PVOID pMineFn)
    {
        Initialize();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(ppRealFn, pMineFn);

        if (DetourTransactionCommit() != NO_ERROR)
        {
            LOGE << "Error hooking API call: " << name;
            return false;
        }

        LOGI << "Hooked API call: " << name;
        return true;
    }

    bool UnhookAPI(const std::string& name, PVOID* ppRealFn, PVOID pMineFn)
    {
        Initialize();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(ppRealFn, pMineFn);

        auto error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            LOGE << "Error unhooking API call: " << name;
            return false;
        }

        LOGI << "Unooked API call: " << name;
        return true;
    }
}