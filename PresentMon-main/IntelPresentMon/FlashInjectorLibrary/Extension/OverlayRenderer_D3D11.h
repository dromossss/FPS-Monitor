#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include "OverlayRenderer.h"

#include <d3dcompiler.h>
#include <d3d11.h>

namespace GfxLayer::Extension
{
	class OverlayRenderer_D3D11 : public OverlayRenderer
	{
	public:
		OverlayRenderer_D3D11(OverlayConfig config, IDXGISwapChain3* pSwapChain, ID3D11Device* pDevice);
		~OverlayRenderer_D3D11() = default;

		void										Render(bool renderBar) override;
		void										Resize(unsigned bufferCount, unsigned width, unsigned height) override;

	private:
		void 										LoadRenderState();

		ComPtr<ID3D11Device>						m_pDevice{};
		ComPtr<ID3D11DeviceContext>					m_pDeferredContext;
		std::vector<ComPtr<ID3D11RenderTargetView>>	m_Rtvs;
		ComPtr<ID3D11Buffer>						m_pVertexBuffer;
		ComPtr<ID3D11Buffer>						m_pIndexBuffer;
		ComPtr<ID3D11Buffer>						m_pConstantBufferBar;
		ComPtr<ID3D11Buffer>						m_pConstantBufferBackground;
		ComPtr<ID3D11VertexShader>					m_pVertexShader;
		ComPtr<ID3D11PixelShader>					m_pPixelShader;
		ComPtr<ID3D11InputLayout>					m_pVertexLayout;

		D3D11_VIEWPORT								m_Viewport{};
	};
}