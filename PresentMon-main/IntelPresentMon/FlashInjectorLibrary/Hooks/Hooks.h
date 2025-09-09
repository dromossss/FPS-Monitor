#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <string>

#define HOOK_API_CALL(PFN, NAME)                                \
    g_DispatchTable->NAME.SetFunctions(                         \
        reinterpret_cast<PFN>(GetProcAddress(hModule, #NAME)),  \
        reinterpret_cast<PFN>(Mine_##NAME)                      \
    );                                                          \
    g_DispatchTable->NAME.Attach();

namespace GfxLayer::Hooks
{
	bool HookAPI(const std::string& name, PVOID* ppRealFn, PVOID pMineFn);
	bool UnhookAPI(const std::string& name, PVOID* ppRealFn, PVOID pMineFn);

	template <typename T>
	class Hook
	{
	public:
		Hook(const std::string& name, T pRealFn = nullptr, T pMineFn = nullptr) :
			m_Name(name), m_pReal(pRealFn), m_pMine(pMineFn)
		{}

		~Hook()
		{
			Detach();
		}

		void SetFunctions(T pRealFn, T pMineFn)
		{
			m_pReal = pRealFn;
			m_pMine = pMineFn;
		}

		bool Attach()
		{
			return HookAPI(m_Name, (PVOID*) & m_pReal, m_pMine);
		}

		bool Detach()
		{
			return UnhookAPI(m_Name, (PVOID*) &m_pReal, m_pMine);
		}

		T Real() const
		{
			return m_pReal;
		}

		T Mine() const
		{
			return m_pMine;
		}

	private:
		std::string m_Name;
		T			m_pReal;
		T			m_pMine;
	};

	// DXGI
	namespace DXGI
	{
		void Hook_DXGI();
	}

	// D3D10
	namespace D3D10
	{
		void Hook_D3D10();
	}

	// D3D11
	namespace D3D11
	{
		void Hook_D3D11();
	}
}