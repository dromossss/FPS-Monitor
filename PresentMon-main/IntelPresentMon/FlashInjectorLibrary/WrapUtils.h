#pragma once
#include "../CommonUtilities/win/WinAPI.h"
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include "Context.h"
#include "Custom/IUnknown_Wrapper.h"

namespace GfxLayer
{
    struct IIDHash 
    {
        size_t operator()(REFIID riid) const noexcept
        {
            const uint32_t* p = reinterpret_cast<const uint32_t*>(&riid);
            return p[0] ^ p[1] ^ p[2] ^ p[3];
        }
    };

    void WrapObject(REFIID riid, void** ppObject);
    void WrapObject_NoStore(REFIID riid, void** ppObject);
    void WrapObjectArray(UINT nObjects, REFIID riid, void** ppObjects);

    template <typename Object>
    Object* UnwrapObject(Object* pWrappedObject)
    {
        if (!pWrappedObject)
        {
            return nullptr;
        }

        IUnknown_Wrapper* pUnknownWrapper = nullptr;
        auto hResult = pWrappedObject->QueryInterface(IID_PPV_ARGS(&pUnknownWrapper));
        if (FAILED(hResult))
        {
            return pWrappedObject;
        }

        return pUnknownWrapper->GetWrappedObjectAs<Object>();
    }

    template <typename Object>
    const Object* UnwrapObject(const Object* pWrappedObject)
    {
        if (!pWrappedObject)
        {
            return nullptr;
        }
        
        ComPtr<IUnknown> pObject = const_cast<IUnknown*>(pWrappedObject);
        IUnknown_Wrapper* pUnknownWrapper = nullptr;
        auto hResult = pObject->QueryInterface(IID_PPV_ARGS(&pUnknownWrapper));
        if (FAILED(hResult))
        {
            return pWrappedObject;
        }

        return pUnknownWrapper->GetWrappedObjectAs<Object>();
    }

    template <>
    const IUnknown* UnwrapObject<IUnknown>(const IUnknown* pWrappedObject);

    template <typename Object>
    Object* const* UnwrapObjects(Object* const* ppObjects, uint32_t len, MemoryPool& memoryPool)
    {
        if ((ppObjects == nullptr) || (len == 0))
        {
            return ppObjects;
        }

        auto*  pUnwrappedObjectsData = memoryPool.NewBuffer(len * sizeof(Object*));
        auto** ppUnwrappedObjects = reinterpret_cast<Object**>(pUnwrappedObjectsData);
        for (uint32_t i = 0; i < len; ++i)
        {
            ppUnwrappedObjects[i] = UnwrapObject<Object>(ppObjects[i]);
        }

        return ppUnwrappedObjects;
        
    }

    template <typename T>
    T* MakeUnwrapStructs(const T* pStructs, size_t len, MemoryPool& memoryPool)
    {
        assert((pStructs != nullptr) && (len > 0));

        auto* pSrcData = reinterpret_cast<const uint8_t*>(pStructs);
        auto* pUnwrappedStructsData = memoryPool.NewBuffer(len * sizeof(T), pSrcData);
        return reinterpret_cast<T*>(pUnwrappedStructsData);
    }

    template <typename T>
    const T* UnwrapStructPtrObjects(const T* pStruct, MemoryPool& memoryPool)
    {
        T* pUnwrappedStruct = nullptr;
        if (pStruct != nullptr)
        {
            pUnwrappedStruct = MakeUnwrapStructs(pStruct, 1, memoryPool);
            UnwrapStructObjects(pUnwrappedStruct, memoryPool);
        }

        return pUnwrappedStruct;
    }

    template <typename T>
    const T* UnwrapStructArrayObjects(const T* pStructs, size_t len, MemoryPool& memoryPool)
    {
        if ((pStructs == nullptr) || (len == 0))
        {
            return pStructs;
        }

        auto* pUnwrappedStructs = MakeUnwrapStructs(pStructs, len, memoryPool);
        for (int i = 0; i < len; ++i)
        {
            UnwrapStructObjects(&pUnwrappedStructs[i], memoryPool);
        }

        return pUnwrappedStructs;
    }

    void PeekVirtualTable(void* object);
}