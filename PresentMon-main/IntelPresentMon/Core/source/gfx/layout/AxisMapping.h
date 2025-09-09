// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Geometry.h"
#include "../base/Geometry.h"
#include "Enums.h"

// these utilities map flex directions to x/y components of geometric primitives
// enabling the flexbox code base to use the same algorithms for both row and column layouts

namespace p2c::gfx::lay::map
{
	FlexDirection CrossDir(FlexDirection dir);
	Vec2 MakeVec2(float inline_, float cross_, FlexDirection dir);
	Dimensions MakeDims(float inline_, float cross_, FlexDirection dir);
	float DimsScalar(const Dimensions& dims, FlexDirection dir);
	std::optional<float>& DimSpecScalar(DimensionsSpec& dims, FlexDirection dir);
	const std::optional<float>& DimSpecScalar(const DimensionsSpec& dims, FlexDirection dir);
	float Vec2Scalar(const Vec2& v, FlexDirection dir);
	float RectNearScalar(const Rect& rect, FlexDirection dir);
	float RectFarScalar(const Rect& rect, FlexDirection dir);
}