#include "WrapUtils.h"
#include "../CommonUtilities/win/WinAPI.h"

#include "Custom/API_D3D11.h"
#include "Generated/API_DXGI_Create_Wrappers.h"

#include <string>
#include <format>
#include <unordered_map>
#include <functional>

#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace GfxLayer
{
    using WrapFn = std::function<void(REFIID, void**)>;
    std::unordered_map<IID, WrapFn, IIDHash> g_CreateWrapperFnTable =
    {
        { IID_IUnknown, WrapIUnknown },
        { IID_ID3D11Device, WrapID3D11Device },
        DXGI_CREATE_WRAPPERS_FN_TABLE
    };

    std::unordered_map<IID, WrapFn, IIDHash> g_CreateWrapperFnTable_NoStore =
    {
        { IID_IUnknown, WrapIUnknown_NoStore },
        { IID_ID3D11Device, WrapID3D11Device_NoStore },
        DXGI_CREATE_WRAPPERS_NO_STORE_FN_TABLE
    };

	void WrapObject(REFIID riid, void** ppObject)
	{
        if ((ppObject == nullptr) || (*ppObject == nullptr))
        {
            return;
        }

        if (g_CreateWrapperFnTable.count(riid))
        {
            auto& wrapObject = g_CreateWrapperFnTable.at(riid);
            wrapObject(riid, ppObject);
        }
        else
        {
            auto guid = std::format("{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
                riid.Data1, riid.Data2, riid.Data3,
                riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3],
                riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
            // LOGE << "Created object with unknown GUID {" << guid << "}";
        }
	}

    void WrapObject_NoStore(REFIID riid, void** ppObject)
	{
        if ((ppObject == nullptr) || (*ppObject == nullptr))
        {
            return;
        }

        if (g_CreateWrapperFnTable.count(riid))
        {
            auto& wrapObjectNoStore = g_CreateWrapperFnTable_NoStore.at(riid);
            wrapObjectNoStore(riid, ppObject);
        }
        else
        {
            auto guid = std::format("{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
                riid.Data1, riid.Data2, riid.Data3,
                riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3],
                riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
            // LOGE << "Created object with unknown GUID {" << guid << "}";
        }
	}

    void WrapObjectArray(UINT nObjects, REFIID riid, void** ppObjects)
    {
        for (UINT i = 0; i < nObjects; i++)
        {
            WrapObject(riid, &ppObjects[i]);
        }
    }

    template <>
    const IUnknown* UnwrapObject<IUnknown>(const IUnknown* pWrappedObject) 
    {
        if (!pWrappedObject)
        {
            return nullptr;
        }
        
        IUnknown_Wrapper* pUnknownWrapper = nullptr;
        ComPtr<IUnknown> pObject = const_cast<IUnknown*>(pWrappedObject);
        auto hResult = pObject->QueryInterface(IID_PPV_ARGS(&pUnknownWrapper));
        if (FAILED(hResult))
        {
            return pWrappedObject;
        }
        
        return pUnknownWrapper->GetWrappedObjectAs<IUnknown>();
    }

    void PeekVirtualTable(void* object)
    {
        // Inspect Virtual Tables of a COM object
        using V = void(*)();
        V* vtbl = *reinterpret_cast<V**>(object);
    }
}