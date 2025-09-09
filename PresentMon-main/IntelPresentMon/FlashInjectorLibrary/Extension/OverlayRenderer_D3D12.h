#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <d3d12.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include "OverlayRenderer.h"

namespace GfxLayer::Extension
{
	class OverlayRenderer_D3D12 : public OverlayRenderer
	{
	public:
		OverlayRenderer_D3D12(OverlayConfig config, IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCmdQueue);
		~OverlayRenderer_D3D12() = default;

		void											Resize(unsigned bufferCount, unsigned width, unsigned height) override;
		void											Render(bool renderBar) override;

	private:
		ComPtr<ID3D12Device>                            m_pDevice{};
		ComPtr<ID3D12CommandQueue>                      m_pCmdQueue{};
		std::vector<ComPtr<ID3D12CommandAllocator>>     m_CmdAllocators{};
		std::vector<ComPtr<ID3D12GraphicsCommandList>>  m_CmdLists{};
		ComPtr<ID3D12DescriptorHeap>                    m_RtvHeap{};
		UINT                                            m_RtvDescriptorSize{};
	};
}