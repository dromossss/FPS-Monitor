#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <comdef.h>
#include <memory>
#include <unordered_map>

namespace GfxLayer
{
    struct IUnknown_Wrapper;

    using IUnknownWrapperMap = std::unordered_map<size_t, std::unique_ptr<IUnknown_Wrapper>>;
    using IUnknownPtr = _com_ptr_t<_com_IIID<IUnknown, &__uuidof(IUnknown)>>;

    const IID IID_IUnknown_Wrapper = { 0xE00BB2CC, 0x162E, 0x4AAD, { 0x97, 0x69, 0xED, 0xE6, 0x91, 0x53, 0x95, 0xF6 } };

    void WrapIUnknown(REFIID riid, void** ppObject);
    void WrapIUnknown_NoStore(REFIID riid, void** ppObject);

    MIDL_INTERFACE("E00BB2CC-162E-4AAD-9769-EDE6915395F6")
	IUnknown_Wrapper: public IUnknown
	{
	public:
        static IUnknown*                    GetRootIUnknown(IUnknown* pUnknown);

		IUnknown_Wrapper(REFIID riid, IUnknown* pObject);
		~IUnknown_Wrapper() = default;

		// IUnknown

        virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void** object) override;
        virtual ULONG STDMETHODCALLTYPE		AddRef() override;
        virtual ULONG STDMETHODCALLTYPE		Release() override;

		// Custom

        REFIID                              GetIID() const;
        IUnknown*                           GetInterfacePtr() const;
        IUnknown*                           GetRootIUnknown();
        void                                SetObject(REFIID riid, IUnknown* pObject);

        template <typename T> 
        T*                                  GetWrappedObjectAs();

        template <typename T>
        const T*                            GetWrappedObjectAs() const;

	private:
		IID									m_IID;
		IUnknownPtr							m_pObject;
        IUnknownWrapperMap                  m_SecondaryWrappers;
	};

    template <typename T>
    T* IUnknown_Wrapper::GetWrappedObjectAs()
    {
        return reinterpret_cast<T*>(m_pObject.GetInterfacePtr());
    }

    template <typename T>
    const T* IUnknown_Wrapper::GetWrappedObjectAs() const
    {
        return reinterpret_cast<const T*>(m_pObject.GetInterfacePtr());
    }
}