// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "PlotElement.h"


namespace p2c::gfx::lay
{
	struct GraphLinePack;

	class HistogramPlotElement : public PlotElement
	{
	public:
		HistogramPlotElement(std::shared_ptr<GraphLinePack> pPack, std::vector<std::string> classes = {});
		~HistogramPlotElement() override;
		void SetValueRangeLeft(float min, float max) override;
		void SetValueRangeRight(float min, float max) override {};
		void SetTimeWindow(float dt) override;
		void SetCountRange(int min, int max);
		int GetMaxCount() const;
	protected:
		void Draw_(Graphics& gfx) const override;
		void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) override;
	private:
		// data
		float minValue = 0;
		float maxValue = 100;
		float timeWindow = 5.f;
		int minCount = 0;
		int maxCount = 100;
		int binCount = 40;
		int hDivs = 20;
		int vDivs = 4;
		Color gridColor{};
		mutable std::vector<int> bins;
		mutable int autoMaxCount = 0;
		std::shared_ptr<GraphLinePack> pPack;
	};
}