// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "DrawablePrimitive.h"
#include "../base/Geometry.h"
#include "../base/ComPtr.h"
#include "ForwardInterfaces.h"
#include <memory>


namespace p2c::gfx::prim
{
	class RectBorderPrimitive : public DrawablePrimitive
	{
	public:
		RectBorderPrimitive(const Rect& rect, const Skirt& stroke, std::shared_ptr<BrushPrimitive> pBrushPrim, Graphics& gfx);
		~RectBorderPrimitive() override;
		void Draw(Graphics& gfx) const override;
	private:
		// independent
		ComPtr<ID2D1GeometryGroup> pGeometry;
		// dependent
		ComPtr<ID2D1GeometryRealization> pRealization;
		std::shared_ptr<BrushPrimitive> pBrushPrim;
		ComPtr<ID2D1Brush> pBrush;
	};
}