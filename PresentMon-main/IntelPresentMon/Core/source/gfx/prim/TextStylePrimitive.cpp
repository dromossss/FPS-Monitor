// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "TextStylePrimitive.h"
#include <Core/source/win/WinAPI.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include "../base/InternalGeometry.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/log/HrLogger.h>
#include <format>
#include "EnumConversion.h"


namespace p2c::gfx::prim
{
	TextStylePrimitive::TextStylePrimitive(const std::wstring& family, float size, Graphics& gfx, Style style, Weight weight)
	{
		pmlog_hr << Write(gfx).CreateTextFormat(
			family.c_str(), nullptr, ConvertWeight(weight), ConvertStyle(style),
			DWRITE_FONT_STRETCH_NORMAL, size, L"", &pFormat
		);
	}

	TextStylePrimitive::~TextStylePrimitive() {}

	void TextStylePrimitive::SetAlignment(Alignment align)
	{
		pmlog_hr << pFormat->SetParagraphAlignment(ConvertAlignment(align));
	}

	void TextStylePrimitive::SetJustification(Justification justify)
	{
		pmlog_hr << pFormat->SetTextAlignment(ConvertJustification(justify));
	}

	TextStylePrimitive::operator IDWriteTextFormat*() const
	{
		return pFormat.Get();
	}
}