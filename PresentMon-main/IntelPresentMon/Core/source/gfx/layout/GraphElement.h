// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "FlexElement.h"
#include "PlotElement.h"


namespace p2c::gfx::lay
{
	class TextElement;
	class LinePlotElement;
	struct GraphLinePack;

	class GraphElement : public FlexElement
	{
	public:
		// functions
		GraphElement(GraphType type_, std::vector<std::shared_ptr<GraphLinePack>> packs_, std::vector<std::string> classes_ = {});
        GraphElement(const GraphElement&) = delete;
        GraphElement& operator=(const GraphElement&) = delete;
		~GraphElement() override;
		static std::shared_ptr<Element> Make(GraphType type, std::vector<std::shared_ptr<GraphLinePack>> packs, std::vector<std::string> classes = {});
		void SetValueRangeLeft(float min, float max);
		void SetValueRangeRight(float min, float max);
		void SetTimeWindow(float dt);
		void SetCountRange(int min, int max);
	protected:
		void Draw_(Graphics& gfx) const override;
		void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) override;
	private:
		// types
		class VirtualLineElement;
		// functions
		void SetAxisLabelsLeft_(float bottom, float top);
		void SetAxisLabelsRight_(float bottom, float top);
		void SetBottomAxisLabels_(float left, float right, std::optional<std::wstring> units = {});
		double GetCurrentTime_() const;
		// data
		std::shared_ptr<PlotElement> pPlot;
		std::shared_ptr<TextElement> pLeftTop;
		std::shared_ptr<TextElement> pLeftBottom;
		std::shared_ptr<TextElement> pRightTop;
		std::shared_ptr<TextElement> pRightBottom;
		std::shared_ptr<TextElement> pBottomLeft;
		std::shared_ptr<TextElement> pBottomRight;
		std::vector<std::shared_ptr<GraphLinePack>> packs;
		mutable std::vector<std::shared_ptr<TextElement>> metricValueReadouts;
		mutable std::vector<std::optional<float>> previousMetricValues;
		bool showCurrentValues;
		// used for left axis in line graph, bin range in histogram
		bool isAutoScalingLeft = false;
		bool isAutoScalingRight = false;
		// only applies to histogram
		bool isAutoScalingCount = true;
		// used for left axis in line graph, bin range in histogram
		mutable std::optional<float> autoLeftMin;
		mutable std::optional<float> autoLeftMax;
		// only applies to line graph
		mutable std::optional<float> autoRightMin;
		mutable std::optional<float> autoRightMax;
		// only applies to histogram
		mutable std::optional<int> autoCountMax;
		double autoScalingShrinkCooldown = 2.5;
		mutable double autoScalingTimestampLeftMin = 0.;
		mutable double autoScalingTimestampLeftMax = 0.;
		mutable double autoScalingTimestampRightMin = 0.;
		mutable double autoScalingTimestampRightMax = 0.;
		mutable double autoScalingTimestampCount = 0.;
	};
}