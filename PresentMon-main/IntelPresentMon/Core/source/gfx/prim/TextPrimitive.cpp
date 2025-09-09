// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "TextPrimitive.h"
#include <Core/source/win/WinAPI.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include "../base/InternalGeometry.h"
#include "EnumConversion.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/log/HrLogger.h>

namespace p2c::gfx::prim
{
	TextPrimitive::TextPrimitive(const std::wstring& text, const TextStylePrimitive& style, const Dimensions& dims, std::shared_ptr<BrushPrimitive> pBrushPrim_, Graphics& gfx, std::optional<Vec2> pos)
		:
		pBrushPrim{ std::move(pBrushPrim_) },
		pBrush{ Brush(*pBrushPrim) },
		position{ pos }
	{
		pmlog_hr << Write(gfx).CreateTextLayout(text.c_str(), (UINT32)text.size(), style, dims.width, dims.height, &pLayout);
	}

	void TextPrimitive::SetMaxDimensions(const Dimensions& dims)
	{
		pmlog_hr << pLayout->SetMaxWidth(dims.width);
		pmlog_hr << pLayout->SetMaxHeight(dims.height);
	}

	Dimensions TextPrimitive::GetActualDimensions() const
	{
		DWRITE_TEXT_METRICS metrics{};
		pmlog_hr << pLayout->GetMetrics(&metrics);
		return { metrics.width, metrics.height };
	}

	Dimensions TextPrimitive::GetMaxDimensions() const
	{
		return { pLayout->GetMaxWidth(), pLayout->GetMaxHeight() };
	}

	void TextPrimitive::SetPosition(const Vec2& pos)
	{
		position = pos;
	}

	void TextPrimitive::SetAlignment(Alignment align)
	{
		pmlog_hr << pLayout->SetParagraphAlignment(ConvertAlignment(align));
	}

	void TextPrimitive::SetJustification(Justification justify)
	{
		pmlog_hr << pLayout->SetTextAlignment(ConvertJustification(justify));
	}

	TextPrimitive::~TextPrimitive() {}

	void TextPrimitive::SetText(const std::wstring& text, Graphics& gfx)
	{
		auto pOld = std::move(pLayout);
		pmlog_hr << Write(gfx).CreateTextLayout(text.c_str(), (UINT32)text.size(), pOld.Get(), pOld->GetMaxWidth(), pOld->GetMaxHeight(), &pLayout);
	}

	void TextPrimitive::Draw(Graphics& gfx) const
	{
		Context(gfx).DrawTextLayout(d2d::Vec2{ *position }, pLayout.Get(), pBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
}