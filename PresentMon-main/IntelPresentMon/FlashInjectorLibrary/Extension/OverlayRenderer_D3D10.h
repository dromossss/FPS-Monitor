#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <d3d10_1.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include "OverlayRenderer.h"

#include <d3dcompiler.h>
#include <d3d10_1.h>
#include <d3d10_1.h>

namespace GfxLayer::Extension
{
	class OverlayRenderer_D3D10 : public OverlayRenderer
	{
	public:
		OverlayRenderer_D3D10(OverlayConfig config, IDXGISwapChain3* pSwapChain, ID3D10Device* pDevice);
		~OverlayRenderer_D3D10() = default;

		void										Render(bool renderBar) override;
		void										Resize(unsigned bufferCount, unsigned width, unsigned height) override;

	private:
		void 										LoadRenderState();

		ComPtr<ID3D10Device>						m_pDevice{};
		std::vector<ComPtr<ID3D10RenderTargetView>>	m_Rtvs;
		ComPtr<ID3D10Buffer>						m_pVertexBuffer;
		ComPtr<ID3D10Buffer>						m_pIndexBuffer;
		ComPtr<ID3D10Buffer>						m_pConstantBufferBar;
		ComPtr<ID3D10Buffer>						m_pConstantBufferBackground;
		ComPtr<ID3D10VertexShader>					m_pVertexShader;
		ComPtr<ID3D10PixelShader>					m_pPixelShader;
		ComPtr<ID3D10InputLayout>					m_pVertexLayout;
		ComPtr<ID3D10StateBlock>					m_pStateBlock;

		D3D10_VIEWPORT								m_Viewport{};
	};
}