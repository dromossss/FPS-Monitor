#include "DXGIOverlay.h"

#include "OverlayRenderer_D3D12.h"
#include "OverlayRenderer_D3D11.h"
#include "OverlayRenderer_D3D10.h"

#define GFXL_OPT_BAR_SIZE "BarSize"
#define GFXL_OPT_BAR_RIGHT_SHIFT "BarRightShift"
#define GFXL_OPT_BAR_COLOR "BarColor"
#define GFXL_OPT_BACKGROUND_COLOR "BackgroundColor"
#define GFXL_OPT_RENDER_BACKGROUND "RenderBackground"

#include <array>
#include <cassert>

namespace GfxLayer
{
	namespace Extension 
	{
		bool Initialize() { return true; }
	}

	std::unique_ptr<Extension::OverlayRenderer> g_pOverlay = nullptr;

	void InitOverlay(IUnknown* pDevice, IDXGISwapChain* pSwapChain)
	{
		using namespace Extension;

		OverlayConfig overlayCfg = {
			.BarSize = 0.25,
			.BarRightShift = 0.5,
			.BarColor = { 1.0f, 1.0f, 1.0f, 1.0f },
			.RenderBackground = false,
			.BackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f }
		};

		auto& opts = Context::GetInstance().GetOptions();
		auto barSize = opts.GetFloat(GFXL_OPT_BAR_SIZE);
		auto barRightShift = opts.GetFloat(GFXL_OPT_BAR_RIGHT_SHIFT);
		if (!std::isnan(barSize))
		{
			overlayCfg.BarSize = barSize;
		}
		if (!std::isnan(barRightShift))
		{
			overlayCfg.BarRightShift = barRightShift;
		}
		overlayCfg.RenderBackground = opts.GetFlag(GFXL_OPT_RENDER_BACKGROUND);

		if (const auto barColor = opts.GetRgb24Color(GFXL_OPT_BAR_COLOR)) {
			std::memcpy(overlayCfg.BarColor, &*barColor, sizeof(*barColor));
		}

		if (const auto bgColor = opts.GetRgb24Color(GFXL_OPT_BACKGROUND_COLOR)) {
			std::memcpy(overlayCfg.BackgroundColor, &*bgColor, sizeof(*bgColor));
		}

		ComPtr<IDXGISwapChain3> pSwapChain3 = nullptr;
		pSwapChain->QueryInterface(IID_PPV_ARGS(&pSwapChain3));

		// Create API-specific overlay renderer

		ComPtr<ID3D12CommandQueue> pCmdQueue;
		auto hr = pDevice->QueryInterface(IID_PPV_ARGS(&pCmdQueue));
		if (SUCCEEDED(hr))
		{
			g_pOverlay = std::make_unique<OverlayRenderer_D3D12>(overlayCfg, pSwapChain3.Get(), pCmdQueue.Get());
			LOGI << "Created DXGI Overlay Renderer (D3D12)";
		}
		else
		{
			ComPtr<ID3D10Device> pD3D10Device;
			hr = pDevice->QueryInterface(IID_PPV_ARGS(&pD3D10Device));
			if (SUCCEEDED(hr))
			{
				g_pOverlay = std::make_unique<OverlayRenderer_D3D10>(overlayCfg, pSwapChain3.Get(), pD3D10Device.Get());
				LOGI << "Created DXGI Overlay Renderer (D3D10)";
			}
			else
			{
				ComPtr<ID3D11Device> pD3D11Device;
				hr = pDevice->QueryInterface(IID_PPV_ARGS(&pD3D11Device));
				if (SUCCEEDED(hr))
				{
					g_pOverlay = std::make_unique<OverlayRenderer_D3D11>(overlayCfg, pSwapChain3.Get(), pD3D11Device.Get());
					LOGI << "Created DXGI Overlay Renderer (D3D11)";
				}
				else
				{
					LOGE << "Failed to create a D3D context.";
					assert(0);
				}
			}
		}

		if (!g_pOverlay)
		{
			LOGE << "Failed to create DXGI Overlay Renderer.";
			assert(0);
		}

		// Initial resize

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		pSwapChain->GetDesc(&swapChainDesc);
		g_pOverlay->Resize(swapChainDesc.BufferCount, swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
	}
}