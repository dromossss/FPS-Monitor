#pragma once
#include "../../CommonUtilities/win/WinAPI.h"
#include <dxgi1_6.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#include "../Context.h"
#include "../NonCopyable.h"

namespace GfxLayer::Extension
{
	void CheckResult(HRESULT result, const char* pMessage);

	struct OverlayConfig
	{
		float BarSize;
		float BarRightShift;
		float BarColor[4];

		bool  RenderBackground;
		float BackgroundColor[4];
	};

	class OverlayRenderer : public NonCopyable
	{
	public:
		OverlayRenderer(OverlayConfig config, IDXGISwapChain3* pSwapChain);
		virtual ~OverlayRenderer() = default;

		virtual void			Resize(unsigned bufferCount, unsigned width, unsigned height);

		void					NewFrame();
		RECT					GetScissorRect() const;
		IDXGISwapChain3*		GetSwapChain() const;
		OverlayConfig			GetConfig() const;

	protected:
		virtual void			Render(bool renderBar) = 0;

	private:
		OverlayConfig			m_Config{};
		ComPtr<IDXGISwapChain3>	m_pSwapChain{};
		RECT					m_ScissorRect{};
	};
}