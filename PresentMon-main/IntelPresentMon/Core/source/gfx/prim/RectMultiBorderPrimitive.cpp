// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "RectMultiBorderPrimitive.h"
#include <Core/source/win/WinAPI.h>
#include <d2d1_3.h>
#include "../base/InternalGeometry.h"
#include <array>
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/log/HrLogger.h>


namespace p2c::gfx::prim
{
	RectMultiBorderPrimitive::RectMultiBorderPrimitive(const Rect& rect, const Skirt& stroke,
		std::shared_ptr<BrushPrimitive> pBrushPrimLeft_,
		std::shared_ptr<BrushPrimitive> pBrushPrimTop_,
		std::shared_ptr<BrushPrimitive> pBrushPrimRight_,
		std::shared_ptr<BrushPrimitive> pBrushPrimBottom_,
		Graphics& gfx)
		:
		left{ std::move(pBrushPrimLeft_) },
		top{ std::move(pBrushPrimTop_) },
		right{ std::move(pBrushPrimRight_) },
		bottom{ std::move(pBrushPrimBottom_) }
	{
		const auto InitGeometry = [this, &gfx](const D2D1_RECT_F& rect, Side& side) {
			pmlog_hr << Factory(gfx).CreateRectangleGeometry(rect, &side.pGeometry);
			pmlog_hr << Context(gfx).CreateFilledGeometryRealization(side.pGeometry.Get(), D2D1_DEFAULT_FLATTENING_TOLERANCE, &side.pRealized);
		};
		InitGeometry({ rect.left, rect.top, rect.right, rect.top + stroke.top }, top);
		InitGeometry({ rect.left, rect.bottom - stroke.bottom, rect.right, rect.bottom }, bottom);
		InitGeometry({ rect.left, rect.top + stroke.top, rect.left + stroke.left, rect.bottom - stroke.bottom }, left);
		InitGeometry({ rect.right - stroke.right, rect.top + stroke.top, rect.right, rect.bottom - stroke.bottom }, right);
	}

	RectMultiBorderPrimitive::~RectMultiBorderPrimitive() {}

	void RectMultiBorderPrimitive::Draw(Graphics& gfx) const
	{
		const auto DrawSide = [this, &gfx](const Side& side) {
			Context(gfx).DrawGeometryRealization(side.pRealized.Get(), side.pBrush.Get());
		};
		DrawSide(left);
		DrawSide(top);
		DrawSide(right);
		DrawSide(bottom);
	}

	RectMultiBorderPrimitive::Side::Side(std::shared_ptr<BrushPrimitive> pBrushPrim_)
		:
		pBrushPrim{ std::move(pBrushPrim_) },
		pBrush{ RectMultiBorderPrimitive::Brush(*pBrushPrim) }
	{}
}