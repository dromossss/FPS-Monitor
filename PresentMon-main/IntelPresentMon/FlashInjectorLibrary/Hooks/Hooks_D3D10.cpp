#include "../Hooks/Hooks.h"
#include "../../CommonUtilities/win/WinAPI.h"
#include "../../CommonUtilities/str/String.h"
#include "../WrapUtils.h"
#include "../Logging.h"

#include "../Custom/Extensions.h"
#include "../Generated/API_DXGI.h"

#include <dxgi1_6.h>
#include <d3d11.h>

using namespace pmon::util;

namespace GfxLayer::Hooks::D3D10
{
    typedef HRESULT(WINAPI* PFN_D3D10_CREATE_DEVICE_AND_SWAPCHAIN)(IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice);

    HRESULT WINAPI Mine_D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice);

    struct DispatchTable
    {
        Hook<PFN_D3D10_CREATE_DEVICE_AND_SWAPCHAIN> D3D10CreateDeviceAndSwapChain{ "D3D10CreateDeviceAndSwapChain" };
    };

    std::unique_ptr<DispatchTable> g_DispatchTable;

    void Hook_D3D10()
    {
        std::wstring systemDir;
        systemDir.resize(MAX_PATH);
        GetSystemDirectoryW(systemDir.data(), (UINT)systemDir.size());

        auto dllPath = systemDir.c_str() + std::wstring(L"\\d3d10.dll");
        auto hModule = LoadLibraryW(dllPath.c_str());
        if (!hModule)
        {
            LOGE << "Failed to load DXGI DLL from " << str::ToNarrow(dllPath);
            exit(1);
        }

        g_DispatchTable = std::make_unique<DispatchTable>();
        HOOK_API_CALL(PFN_D3D10_CREATE_DEVICE_AND_SWAPCHAIN, D3D10CreateDeviceAndSwapChain);
    }

    HRESULT WINAPI Mine_D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->D3D10CreateDeviceAndSwapChain.Real()(
            pAdapter,
            DriverType,
            Software,
            Flags,
            SDKVersion,
            pSwapChainDesc,
            ppSwapChain,
            ppDevice
        );

        if (callScope == 1)
        {
            LOGI << "Intercepted DXGI SwapChain";
            if (ppSwapChain)
            {
                WrapObject(IID_IDXGISwapChain4, (void**) ppSwapChain);
            }
            PostCall<API_D3D10_CREATE_DEVICE_AND_SWAPCHAIN>::Run(result, pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
        }

        ctx.DecrementCallScope();
        return result;
    }
}