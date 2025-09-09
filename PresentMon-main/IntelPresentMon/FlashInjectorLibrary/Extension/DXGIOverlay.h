#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include "../../CommonUtilities/str/String.h"
#include <d3d10_1.h>
#include <d3d11.h>
#include <d3d12.h>

#include "../Context.h"
#include "../NonCopyable.h"
#include "../WrapUtils.h"
#include "../Custom/PrePostCalls.h"

#include "OverlayRenderer.h"
#include "../Logging.h"

namespace GfxLayer
{
    extern std::unique_ptr<Extension::OverlayRenderer> g_pOverlay;

    void InitOverlay(IUnknown* pDevice, IDXGISwapChain* pSwapChain);

    template<>
    struct PostCall<API_IDXGIFACTORY_CREATESWAPCHAIN>
	{
		static void Run(HRESULT& result, IDXGIFactory_Wrapper* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
		{
			if (FAILED(result) || (ppSwapChain == nullptr) || (*ppSwapChain == nullptr))
			{
				return;
			}

			auto** ppSwapChainWrapper = (IDXGISwapChain3_Wrapper**) ppSwapChain;
			auto*  pSwapChain = (*ppSwapChainWrapper)->GetWrappedObjectAs<IDXGISwapChain3>();
            InitOverlay(UnwrapObject<IUnknown>(pDevice), pSwapChain);
		}
	};

    template <>
    struct PostCall<API_IDXGIFACTORY2_CREATESWAPCHAINFORHWND>
    {
        static void Run(HRESULT& result, IDXGIFactory2_Wrapper* pFactory, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain)
		{
			if (FAILED(result) || (ppSwapChain == nullptr) || (*ppSwapChain == nullptr))
			{
				return;
			}

			auto** ppSwapChainWrapper = (IDXGISwapChain3_Wrapper**) ppSwapChain;
			auto*  pSwapChain = (*ppSwapChainWrapper)->GetWrappedObjectAs<IDXGISwapChain3>();
            InitOverlay(UnwrapObject<IUnknown>(pDevice), pSwapChain);
		}
    };

    template <>
    struct PostCall<API_D3D10_CREATE_DEVICE_AND_SWAPCHAIN>
	{
		static void Run(HRESULT& result, IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice)
		{
			if (FAILED(result) || (ppSwapChain == nullptr) || (*ppSwapChain == nullptr))
			{
				return;
			}

			auto** ppSwapChainWrapper = (IDXGISwapChain3_Wrapper**) ppSwapChain;
			auto*  pSwapChain = (*ppSwapChainWrapper)->GetWrappedObjectAs<IDXGISwapChain3>();
			InitOverlay(UnwrapObject<IUnknown>(*ppDevice), pSwapChain);
		}
	};

	template <>
	struct PostCall<API_D3D11_CREATE_DEVICE_AND_SWAPCHAIN>
	{
		static void Run(HRESULT& result, IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
		{
			if (FAILED(result) || (ppSwapChain == nullptr) || (*ppSwapChain == nullptr))
			{
				return;
			}

			auto** ppSwapChainWrapper = (IDXGISwapChain3_Wrapper**)ppSwapChain;
			auto* pSwapChain = (*ppSwapChainWrapper)->GetWrappedObjectAs<IDXGISwapChain3>();
			InitOverlay(UnwrapObject<IUnknown>(*ppDevice), pSwapChain);
		}
	};

    template <>
    struct PostCall<API_IDXGISWAPCHAIN_RESIZEBUFFERS>
    {
        static void Run(HRESULT& result, IDXGISwapChain_Wrapper* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
        {
			LOGI << "Resizing DXGISwapChain buffers to " << Width << "x" << Height << " (BufferCount: " << BufferCount << ")";
            g_pOverlay->Resize(BufferCount, Width, Height);
        }
    };

	template <>
	struct PostCall<API_IDXGISWAPCHAIN3_RESIZEBUFFERS1>
	{
		static void Run(HRESULT& result, IDXGISwapChain_Wrapper* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue)
		{
			LOGI << "Resizing DXGISwapChain buffers to " << Width << "x" << Height << " (BufferCount: " << BufferCount << ")";
			g_pOverlay->Resize(BufferCount, Width, Height);
		}
	};

    template <>
    struct PreCall<API_IDXGISWAPCHAIN_PRESENT>
    {
        static bool Run(HRESULT& result, IDXGISwapChain_Wrapper* pSwapChain, UINT SyncInterval, UINT Flags)
        {
            g_pOverlay->NewFrame();
            return false;
        }
    };

	template <>
	struct PreCall<API_IDXGISWAPCHAIN1_PRESENT1>
	{
		static bool Run(HRESULT& result, IDXGISwapChain_Wrapper* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters)
		{
			g_pOverlay->NewFrame();
			return false;
		}
	};
}