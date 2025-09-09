#include "../WrapUtils.h"
#include "../Context.h"
#include "IUnknown_Wrapper.h"
#include <cassert>

namespace GfxLayer
{
	IUnknown* IUnknown_Wrapper::GetRootIUnknown(IUnknown* pUnknown)
	{
		IUnknown* pRootUnknown = nullptr;
		HRESULT hr = pUnknown->QueryInterface(IID_PPV_ARGS(&pRootUnknown));
		if (pRootUnknown)
		{
			pRootUnknown->Release();
			assert(hr == S_OK);
		}
		return pRootUnknown;
	}

	IUnknown_Wrapper::IUnknown_Wrapper(REFIID riid, IUnknown* pObject):
		m_IID(riid),
		m_pObject(pObject, false)
	{}

	void IUnknown_Wrapper::SetObject(REFIID riid, IUnknown* pObject)
	{
		m_IID = riid;
		m_pObject.Attach(pObject);
	}

	HRESULT STDMETHODCALLTYPE IUnknown_Wrapper::QueryInterface(REFIID riid, void** object)
	{
		if (!object)
		{
			return E_FAIL;
		}

		if (IsEqualIID(riid, IID_IUnknown_Wrapper))
		{
			(*object) = this;
			return S_OK;
		}

		// Return the wrapped object when querying for IID_Unknown
		auto targetIID = riid;
		if (IsEqualIID(riid, IID_IUnknown))
		{
			targetIID = m_IID;
		}

		void* pObject = nullptr;
		auto  hResult = m_pObject->QueryInterface(targetIID, &pObject);
		if (SUCCEEDED(hResult))
		{
			IIDHash iidHasher;
			auto iidHash = iidHasher(targetIID);
			
			if (IsEqualGUID(targetIID, m_IID))
			{
				(*object) = this;
			}
			else if (m_SecondaryWrappers.contains(iidHash))
			{
				(*object) = m_SecondaryWrappers.at(iidHash).get();
			}
			else
			{
				WrapObject_NoStore(targetIID, &pObject);
				(*object) = pObject;

				m_SecondaryWrappers.emplace(iidHash, reinterpret_cast<IUnknown_Wrapper*>(pObject));
			}
		}

		return hResult;
	}

	ULONG STDMETHODCALLTYPE IUnknown_Wrapper::AddRef()
	{
		auto result = m_pObject->AddRef();
		return result;
	}

	ULONG STDMETHODCALLTYPE IUnknown_Wrapper::Release()
	{
		m_pObject->AddRef();
		auto result = m_pObject->Release();
		if (result == 1) 
		{
			Context::GetInstance().RemoveWrapper(this);
		}

		result = m_pObject->Release();
		return result;
	}

	REFIID IUnknown_Wrapper::GetIID() const
	{
		return m_IID;
	}

	IUnknown* IUnknown_Wrapper::GetInterfacePtr() const
	{
		return m_pObject.GetInterfacePtr();
	}

	IUnknown* IUnknown_Wrapper::GetRootIUnknown()
	{
		return IUnknown_Wrapper::GetRootIUnknown(GetInterfacePtr());
	}

	void WrapIUnknown(REFIID riid, void** ppObject)
	{
		auto&  ctx = Context::GetInstance();
        auto** ppObjectToWrap = reinterpret_cast<IUnknown**>(ppObject);
        auto*  pUnknownWrapper = ctx.FindWrapper(*ppObjectToWrap);
        if (!pUnknownWrapper)
        {
            pUnknownWrapper = new IUnknown_Wrapper(riid, *ppObjectToWrap);
            ctx.AddWrapper(pUnknownWrapper);
        }
        (*ppObject) = pUnknownWrapper;
	}

	void WrapIUnknown_NoStore(REFIID riid, void** ppObject)
	{
        auto** ppObjectToWrap = reinterpret_cast<IUnknown**>(ppObject);
        (*ppObject) = new IUnknown_Wrapper(riid, *ppObjectToWrap);
	}
}