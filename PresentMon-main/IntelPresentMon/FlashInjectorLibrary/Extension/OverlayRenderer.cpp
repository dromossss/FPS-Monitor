#include "OverlayRenderer.h"
#include <cassert>
#include "../Logging.h"

namespace GfxLayer::Extension
{
	void CheckResult(HRESULT result, const char* pMessage)
	{
		if (FAILED(result))
		{
			LOGE << "Error: " << pMessage << " (0x" << std::hex << result << ")";
			assert(0);
		}
	}

	OverlayRenderer::OverlayRenderer(OverlayConfig config, IDXGISwapChain3* pSwapChain):
		m_pSwapChain(pSwapChain),
		m_Config(config)
	{}

	RECT OverlayRenderer::GetScissorRect() const
	{
		return m_ScissorRect;
	}

	IDXGISwapChain3* OverlayRenderer::GetSwapChain() const
	{
		return m_pSwapChain.Get();
	}

	OverlayConfig OverlayRenderer::GetConfig() const
	{
		return m_Config;
	}

	void OverlayRenderer::Resize(unsigned bufferCount, unsigned width, unsigned height)
	{
		float rectWidth = width * m_Config.BarSize;
		m_ScissorRect.left = LONG((width - rectWidth) * m_Config.BarRightShift);
		m_ScissorRect.top = 0;
		m_ScissorRect.right = LONG(m_ScissorRect.left + rectWidth);
		m_ScissorRect.bottom = height;
	}

	void OverlayRenderer::NewFrame()
	{
		static unsigned s_frameCounter = 0;
		static bool s_mouseClicked = false;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			if (!s_mouseClicked)
			{
				s_frameCounter = 16;
			}
			s_mouseClicked = true;
		}
		else
		{
			s_mouseClicked = false;
		}

		if (s_frameCounter > 0)
		{
			Render(true);
			--s_frameCounter;
		}
		else if (m_Config.RenderBackground)
		{
			Render(false);
		}
	}
}

