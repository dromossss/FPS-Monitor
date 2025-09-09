// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Geometry.h"
#include <d2d1_2.h>

namespace p2c::gfx::d2d
{
	// allows smooth conversion from our Color type to d2d color type
	struct Color : public D2D_COLOR_F
	{
	public:
		Color() = default;
		Color(const gfx::Color& c) : D2D_COLOR_F{ c.r, c.g, c.b, c.a } {}
	};

	struct Rect : public D2D1_RECT_F
	{
	public:
		Rect() = default;
		Rect(const gfx::Rect& r) : D2D1_RECT_F{ .left = r.left, .top = r.top, .right = r.right, .bottom = r.bottom } {}
	};

	struct Vec2 : public D2D1_POINT_2F
	{
	public:
		Vec2() = default;
		Vec2(const gfx::Vec2& v) : D2D1_POINT_2F{ .x = v.x, .y = v.y } {}
	};
}
