// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "BrushPrimitive.h"
#include <Core/source/win/WinAPI.h>
#include <d2d1_3.h>
#include "../base/InternalGeometry.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/log/HrLogger.h>

namespace p2c::gfx::prim
{
	BrushPrimitive::BrushPrimitive(const Color& color_, Graphics& gfx)
		:
		color{ color_ }
	{
		pmlog_hr << Context(gfx).CreateSolidColorBrush(d2d::Color{ color }, &pBrush);
	}

	void BrushPrimitive::SetColor(const Color& color_)
	{
		color = color_;
		pBrush->SetColor(d2d::Color{ color });
	}

	BrushPrimitive::~BrushPrimitive() {}

	ComPtr<ID2D1Brush> BrushPrimitive::GetInterface() const
	{
		ComPtr<ID2D1Brush> pBase;
		pBrush.As(&pBase);
		return pBase;
	}
}