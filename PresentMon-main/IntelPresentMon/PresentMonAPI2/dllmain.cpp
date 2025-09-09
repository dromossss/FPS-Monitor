#include "../CommonUtilities/win/WinAPI.h"
#include "../CommonUtilities/log/GlobalPolicy.h"

using namespace pmon::util;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        log::GlobalPolicy::Get().SetSubsystem(log::Subsystem::Middleware);
#ifdef NDEBUG
        // start with logging disabled initially in release builds
        // opt in with internal C++ calls or via C-Api diagnostic layer
        log::GlobalPolicy::Get().SetLogLevel(log::Level::None);
#endif
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

