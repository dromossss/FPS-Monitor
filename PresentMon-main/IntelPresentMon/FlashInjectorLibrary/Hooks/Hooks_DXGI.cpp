#include "../Hooks/Hooks.h"
#include "../../CommonUtilities/win/WinAPI.h"
#include "../../CommonUtilities/str/String.h"
#include "../WrapUtils.h"
#include "../Logging.h"

#include "../Custom/Extensions.h"
#include "../Generated/API_DXGI.h"

#include <dxgi1_6.h>

using namespace pmon::util;

namespace GfxLayer::Hooks::DXGI
{
    typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY)(const IID& riid, void** ppFactory);
    typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY1)(const IID& riid, void** ppFactory);
    typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY2)(UINT Flags, const IID& riid, void** ppFactory);
    typedef HRESULT(WINAPI* PFN_DXGI_DECLARE_ADAPTER_REMOVAL_SUPPORT)();
    typedef HRESULT(WINAPI* PFN_DXGI_GET_DEBUG_INTERFACE1)(UINT Flags, const IID& riid, void** ppDebug);
    typedef HRESULT(WINAPI* PFN_DXGI_D3D10_CREATE_DEVICE)(HMODULE d3d10core, IDXGIFactory* factory, IDXGIAdapter* adapter, UINT flags, DWORD arg5, void** device);

    HRESULT WINAPI Mine_CreateDXGIFactory(const IID& riid, void** ppFactory);
    HRESULT WINAPI Mine_CreateDXGIFactory1(const IID& riid, void** ppFactory);
    HRESULT WINAPI Mine_CreateDXGIFactory2(UINT Flags, const IID& riid, void** ppFactory);
    HRESULT WINAPI Mine_DXGIDeclareAdapterRemovalSupport();
    HRESULT WINAPI Mine_DXGIGetDebugInterface1(UINT Flags, const IID& riid, void** ppDebug);
    HRESULT WINAPI Mine_DXGID3D10CreateDevice(HMODULE d3d10core, IDXGIFactory* factory, IDXGIAdapter* adapter, UINT flags, DWORD arg5, void** device);

    struct DispatchTable
    {
        Hook<PFN_CREATE_DXGI_FACTORY>                   CreateDXGIFactory { "CreateDXGIFactory" };
        Hook<PFN_CREATE_DXGI_FACTORY1>                  CreateDXGIFactory1 { "CreateDXGIFactory1" };
        Hook<PFN_CREATE_DXGI_FACTORY2>                  CreateDXGIFactory2 { "CreateDXGIFactory2" };
        Hook<PFN_DXGI_DECLARE_ADAPTER_REMOVAL_SUPPORT>  DXGIDeclareAdapterRemovalSupport { "DXGIDeclareAdapterRemovalSupport" };
        Hook<PFN_DXGI_GET_DEBUG_INTERFACE1>             DXGIGetDebugInterface1 { "DXGIGetDebugInterface1" };
        Hook<PFN_DXGI_D3D10_CREATE_DEVICE>              DXGID3D10CreateDevice { "DXGID3D10CreateDevice" };
    };

    std::unique_ptr<DispatchTable> g_DispatchTable;

    void Hook_DXGI()
    {
        std::wstring systemDir;
        systemDir.resize(MAX_PATH);
        GetSystemDirectoryW(systemDir.data(), (UINT)systemDir.size());

        auto dllPath = systemDir.c_str() + std::wstring(L"\\dxgi.dll");
        auto hModule = LoadLibraryW(dllPath.c_str());
        if (!hModule)
        {
            LOGE << "Failed to load DXGI DLL from " << str::ToNarrow(dllPath);
            exit(1);
        }

        g_DispatchTable = std::make_unique<DispatchTable>();
        HOOK_API_CALL(PFN_CREATE_DXGI_FACTORY, CreateDXGIFactory);
        HOOK_API_CALL(PFN_CREATE_DXGI_FACTORY1, CreateDXGIFactory1);
        HOOK_API_CALL(PFN_CREATE_DXGI_FACTORY2, CreateDXGIFactory2);
        HOOK_API_CALL(PFN_DXGI_DECLARE_ADAPTER_REMOVAL_SUPPORT, DXGIDeclareAdapterRemovalSupport);
        HOOK_API_CALL(PFN_DXGI_GET_DEBUG_INTERFACE1, DXGIGetDebugInterface1);
        HOOK_API_CALL(PFN_DXGI_D3D10_CREATE_DEVICE, DXGID3D10CreateDevice);
    }

    HRESULT WINAPI Mine_CreateDXGIFactory(const IID& riid, void** ppFactory)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->CreateDXGIFactory.Real()(
            riid,
            ppFactory
        );

        if (callScope == 1)
        {
            LOGI << "Intercepted DXGI Factory";
            if (ppFactory)
            {
                WrapObject(IID_IDXGIFactory7, ppFactory);
            }
        }

        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_CreateDXGIFactory1(const IID& riid, void** ppFactory)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->CreateDXGIFactory1.Real()(
            riid,
            ppFactory
        );

        if (callScope == 1)
        {
            LOGI << "Intercepted DXGI Factory1";
            if (ppFactory)
            {
                WrapObject(IID_IDXGIFactory7, ppFactory);
            }
        }

        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_CreateDXGIFactory2(UINT Flags, const IID& riid, void** ppFactory)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->CreateDXGIFactory2.Real()(
            Flags,
            riid,
            ppFactory
        );

        if (callScope == 1)
        {
            LOGI << "Intercepted DXGI Factory2";
            if (ppFactory)
            {
                WrapObject(IID_IDXGIFactory7, ppFactory);
            }
        }

        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_DXGIDeclareAdapterRemovalSupport()
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->DXGIDeclareAdapterRemovalSupport.Real()();
        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_DXGIGetDebugInterface1(UINT Flags, const IID& riid, void** ppDebug)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->DXGIGetDebugInterface1.Real()(
            Flags,
            riid,
            ppDebug
        );

        if ((callScope == 1) && ppDebug)
        {
            WrapObject(riid, ppDebug);
        }

        ctx.DecrementCallScope();
        return result;
    }

    HRESULT WINAPI Mine_DXGID3D10CreateDevice(HMODULE d3d10core, IDXGIFactory* factory, IDXGIAdapter* adapter, UINT flags, DWORD arg5, void** device)
    {
        auto& ctx = Context::GetInstance();
        auto  callScope = ctx.IncrementCallScope();
        auto  result = g_DispatchTable->DXGID3D10CreateDevice.Real()(
            d3d10core,
            factory,
            adapter,
            flags,
            arg5,
            device
        );
        ctx.DecrementCallScope();
        return result;
    }
}