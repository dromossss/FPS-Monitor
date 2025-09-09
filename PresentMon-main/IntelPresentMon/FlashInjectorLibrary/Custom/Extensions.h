#pragma once

#include "PrePostCalls.h"

// List of extensions to enable
#include "../Extension/DXGIOverlay.h"

namespace GfxLayer
{
    namespace Extension
    {
        extern bool Initialize();
    }

    template <>
    struct PreCall<API_IDXGISWAPCHAIN_GETBUFFER>
    {
        static bool Run(HRESULT& result, IDXGISwapChain_Wrapper* pObject, UINT Buffer, REFIID riid, void** ppSurface)
        {
            result = pObject->GetWrappedObjectAs<IDXGISwapChain>()->GetBuffer(
                Buffer,
                riid,
                ppSurface
            );

            if (SUCCEEDED(result))
            {
                auto targetIID = riid;

                // GfxBench calls GetBuffer with IID_IUnknown (?)
                if (IsEqualIID(riid, IID_IUnknown))
                {
                    targetIID = pObject->GetIID();
                }

                WrapObject(targetIID, ppSurface);
            }
            return true;
        }
    };
}