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

namespace GfxLayer::Hooks::D3D11
{
    typedef HRESULT(WINAPI* PFN_D3D11_CREATE_DEVICE)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
    typedef HRESULT(WINAPI* PFN_D3D11_CREATE_DEVICE_AND_SWAPCHAIN)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
    
    HRESULT WINAPI Mine_D3D11CreateDevice(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
    HRESULT WINAPI Mine_D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);

    struct DispatchTable
    {
        Hook<PFN_D3D11_CREATE_DEVICE>               D3D11CreateDevice{ "D3D11CreateDevice" };
        Hook<PFN_D3D11_CREATE_DEVICE_AND_SWAPCHAIN> D3D11CreateDeviceAndSwapChain{ "D3D11CreateDeviceAndSwapChain" };
    };

    std::unique_ptr<DispatchTable> g_DispatchTable;

    void Hook_D3D11()
    {
        std::wstring systemDir;
        systemDir.resize(MAX_PATH);
        GetSystemDirectoryW(systemDir.data(), (UINT)systemDir.size());

        auto dllPath = systemDir.c_str() + std::wstring(L"\\d3d11.dll");
        auto hModule = LoadLibraryW(dllPath.c_str());
        if (!hModule)
        {
            LOGE << "Failed to load DXGI DLL from " << str::ToNarrow(dllPath);
            exit(1);
        }

        g_DispatchTable = std::make_unique<DispatchTable>();
        HOOK_API_CALL(PFN_D3D11_CREATE_DEVICE, D3D11CreateDevice);
        HOOK_API_CALL(PFN_D3D11_CREATE_DEVICE_AND_SWAPCHAIN, D3D11CreateDeviceAndSwapChain);
    }

    HRESULT WINAPI Mine_D3D11CreateDevice(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();

        HRESULT result = S_OK;
        if (callScope == 1)
        {
            LOGI << "Intercepted D3D11 CreateDevice";

            // Update flags to enable creation of ID3D11DeviceContext (remove D3D11_CREATE_DEVICE_SINGLETHREADED)
            Flags &= ~D3D11_CREATE_DEVICE_SINGLETHREADED;

            result = g_DispatchTable->D3D11CreateDevice.Real()(
                UnwrapObject<IDXGIAdapter>(pAdapter),
                DriverType,
                Software,
                Flags,
                pFeatureLevels,
                FeatureLevels,
                SDKVersion,
                ppDevice,
                pFeatureLevel,
                ppImmediateContext
            );
            if (SUCCEEDED(result) && ppDevice)
            {
                WrapObject(IID_ID3D11Device, (void**)ppDevice);
            }
        }
        else
        {
            result = g_DispatchTable->D3D11CreateDevice.Real()(
                pAdapter,
                DriverType,
                Software,
                Flags,
                pFeatureLevels,
                FeatureLevels,
                SDKVersion,
                ppDevice,
                pFeatureLevel,
                ppImmediateContext
            );
        }

        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();

        HRESULT result = S_OK;
        if (callScope == 1)
        {
            // Update flags to enable creation of ID3D11DeviceContext (remove D3D11_CREATE_DEVICE_SINGLETHREADED)
            Flags &= ~D3D11_CREATE_DEVICE_SINGLETHREADED;

            result = g_DispatchTable->D3D11CreateDeviceAndSwapChain.Real()(
                UnwrapObject<IDXGIAdapter>(pAdapter),
                DriverType,
                Software,
                Flags,
                pFeatureLevels,
                FeatureLevels,
                SDKVersion,
                pSwapChainDesc,
                ppSwapChain,
                ppDevice,
                pFeatureLevel,
                ppImmediateContext
            );
            if (SUCCEEDED(result) && ppDevice)
            {
                WrapObject(IID_ID3D11Device, (void**)ppDevice);
            }
            if (SUCCEEDED(result) && ppSwapChain)
            {
                LOGI << "Intercepted DXGI SwapChain";
                WrapObject(IID_IDXGISwapChain4, (void**)ppSwapChain);
            }
            PostCall<API_D3D11_CREATE_DEVICE_AND_SWAPCHAIN>::Run(result, pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
        }
        else
        {
            result = g_DispatchTable->D3D11CreateDeviceAndSwapChain.Real()(
                pAdapter,
                DriverType,
                Software,
                Flags,
                pFeatureLevels,
                FeatureLevels,
                SDKVersion,
                pSwapChainDesc,
                ppSwapChain,
                ppDevice,
                pFeatureLevel,
                ppImmediateContext
             );
        }

        ctx.DecrementCallScope();
        return result;
    }
}