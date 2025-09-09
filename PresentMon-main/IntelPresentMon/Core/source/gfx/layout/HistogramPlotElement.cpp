// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "HistogramPlotElement.h"
#include "TextElement.h"
#include "GraphData.h"
#include <cmath>
#include <algorithm>
#include <functional>
#include <ranges>
#include <Core/source/infra/Logging.h>
#include <Core/source/gfx/layout/style/StyleProcessor.h>


namespace p2c::gfx::lay
{
	using namespace std::string_literals;

	HistogramPlotElement::HistogramPlotElement(std::shared_ptr<GraphLinePack> pPack_, std::vector<std::string> classes_)
		:
		PlotElement{ {}, std::move(classes_) },
		pPack{ pPack_ }
	{}

	HistogramPlotElement::~HistogramPlotElement() {}

	void HistogramPlotElement::Draw_(Graphics& gfx) const
	{
		const auto& data = *pPack->data;

		const auto port = GetContentRect();
		const auto dims = port.GetDimensions();
		const auto yScale = dims.height / float(maxCount - minCount);
		const auto yBias = minCount;
		const auto yOffset = port.bottom;

		// initialize histogram bins
		bins.resize(binCount);
		std::ranges::fill(bins, 0);
		const auto binSize = (maxValue - minValue) / float(binCount);
		autoMaxCount = 0; // for autosizing

		// sort datapoints into bins
		const auto dataSize = data.Size();
		const auto cutoff = (dataSize > 0 ? data.Front().time : 0.f) - timeWindow;
		for (size_t i = 0; i < dataSize; i++)
		{
			const auto& d = data[i];
			// exit early if outside of time window of considering, or not valid data
			if (d.time <= cutoff || !d.value)
			{
				break;
			}
			const int iBin = int((*d.value - minValue) / binSize);
			if (iBin >= 0 && iBin < binCount)
			{
				autoMaxCount = std::max(autoMaxCount, bins[size_t(iBin)] += 1);
			}
		}

		DrawGrid(gfx, port, hDivs, vDivs, gridColor);

		// helper to generate geometry for filled and line rectangle drawing
		const auto rectWidth = dims.width / float(binCount);
		const auto MakeBar = [&](size_t i, int count) -> Rect
		{
			return {
				port.left + float(i) * rectWidth,
				yOffset - yScale * (float(count) - yBias),
				port.left + float(i + 1) * rectWidth,
				port.bottom
			};
		};

		// fill
		if (pPack->fillColor.IsVisible())
		{
			gfx.FastTriangleBatchStart(port);
			for (size_t i = 0; i < bins.size(); i++)
			{
				gfx.FastRectEmit(MakeBar(i, bins[i]), pPack->fillColor);
			}
			gfx.FastBatchEnd();
		}

		// outline
		if (pPack->lineColor.IsVisible())
		{
			gfx.FastLineBatchStart(port);
			for (size_t i = 0; i < bins.size(); i++)
			{
				gfx.FastLineRectTopEmit(MakeBar(i, bins[i]), pPack->lineColor);
			}
			gfx.FastBatchEnd();
		}
	}

	void HistogramPlotElement::SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx)
	{
		gridColor = sp.Resolve<sty::at::graphGridColor>();
		hDivs = sp.Resolve<sty::at::graphHorizontalDivs>();
		vDivs = sp.Resolve<sty::at::graphVerticalDivs>();
		binCount = sp.Resolve<sty::at::graphBinCount>();
		PlotElement::SetPosition_(pos, dimensions, sp, gfx);
	}

	void HistogramPlotElement::SetValueRangeLeft(float min, float max)
	{
		minValue = min;
		maxValue = max;
	}

	void HistogramPlotElement::SetTimeWindow(float dt)
	{
		timeWindow = dt;
	}

	void HistogramPlotElement::SetCountRange(int min, int max)
	{
		minCount = min;
		maxCount = max;
	}

	int HistogramPlotElement::GetMaxCount() const
	{
		return autoMaxCount;
	}
}