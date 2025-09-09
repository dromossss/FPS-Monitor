// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "FlexElement.h"


namespace p2c::gfx::lay
{
	class PlotElement : public FlexElement
	{
	protected:
		using FlexElement::FlexElement;
		static void DrawGrid(Graphics& gfx, const Rect& port, int hDivs, int vDivs, Color gridColor);
	public:
		virtual void SetValueRangeLeft(float min, float max) = 0;
		virtual void SetValueRangeRight(float min, float max) = 0;
		virtual void SetTimeWindow(float dt) = 0;
	};
}