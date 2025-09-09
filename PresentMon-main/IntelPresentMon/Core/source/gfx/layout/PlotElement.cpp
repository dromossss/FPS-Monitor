// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "PlotElement.h"


namespace p2c::gfx::lay
{
	// TODO: better handling of clipping edges (floor + 1)
	void PlotElement::DrawGrid(Graphics& gfx, const Rect& port, int hDivs, int vDivs, Color gridColor)
	{
		if (gridColor.a == 0.f)
		{
			return;
		}

		const auto dims = port.GetDimensions();
		gfx.FastLineBatchStart(port);
		for (int i = 1; i < vDivs; i++)
		{
			const auto y = port.top + float(i) * (dims.height / float(vDivs));
			gfx.FastLineStart({ port.left, y }, gridColor);
			gfx.FastLineEnd({ port.right, y });
		}
		for (int i = 1; i < hDivs; i++)
		{
			auto x = port.left + float(i) * (dims.width / float(hDivs));
			gfx.FastLineStart({ x, port.top }, gridColor);
			gfx.FastLineEnd({ x, port.bottom });
		}
		gfx.FastBatchEnd();
	}
}