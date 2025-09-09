// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "PlotElement.h"


namespace p2c::gfx::lay
{
	struct GraphLinePack;

	class LinePlotElement : public PlotElement
	{
	public:
		LinePlotElement(std::vector<std::shared_ptr<GraphLinePack>> pack, std::vector<std::string> classes = {});
		~LinePlotElement() override;
		void SetValueRangeLeft(float min, float max) override;
		void SetValueRangeRight(float min, float max) override;
		void SetTimeWindow(float dt) override;
	protected:
		void Draw_(Graphics& gfx) const override;
		void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) override;
	private:
		// data
		float minValueLeft = 0;
		float maxValueLeft = 100;
		float minValueRight = 0;
		float maxValueRight = 100;
		float timeWindow = .5f;
		int hDivs = 20;
		int vDivs = 4;
		Color gridColor;
		bool aa = false;
		bool hasRightAxis = false;
		std::vector<std::shared_ptr<GraphLinePack>> packs;
	};
}