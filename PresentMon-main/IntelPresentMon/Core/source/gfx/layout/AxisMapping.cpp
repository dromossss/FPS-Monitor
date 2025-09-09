// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "AxisMapping.h"

namespace p2c::gfx::lay::map
{
	FlexDirection CrossDir(FlexDirection dir)
	{
		return dir == FlexDirection::Column ? FlexDirection::Row : FlexDirection::Column;
	}
	Vec2 MakeVec2(float inline_, float cross_, FlexDirection dir)
	{
		if (dir == FlexDirection::Column)
		{
			return { cross_, inline_ };
		}
		else
		{
			return { inline_,cross_ };
		}
	}
	Dimensions MakeDims(float inline_, float cross_, FlexDirection dir)
	{
		if (dir == FlexDirection::Column)
		{
			return { cross_, inline_ };
		}
		else
		{
			return { inline_, cross_ };
		}
	}
	float DimsScalar(const Dimensions& dims, FlexDirection dir)
	{
		return dir == FlexDirection::Column ? dims.height : dims.width;
	}
	const std::optional<float>& DimSpecScalar(const DimensionsSpec& dims, FlexDirection dir)
	{
		return DimSpecScalar(const_cast<DimensionsSpec&>(dims), dir);
	}
	std::optional<float>& DimSpecScalar(DimensionsSpec& dims, FlexDirection dir)
	{
		return dir == FlexDirection::Column ? dims.height : dims.width;
	}
	float Vec2Scalar(const Vec2& v, FlexDirection dir)
	{
		return dir == FlexDirection::Column ? v.y : v.x;
	}
	float RectNearScalar(const Rect& rect, FlexDirection dir)
	{
		return dir == FlexDirection::Column ? rect.top : rect.left;
	}
	float RectFarScalar(const Rect& rect, FlexDirection dir)
	{
		return dir == FlexDirection::Column ? rect.bottom : rect.right;
	}
}