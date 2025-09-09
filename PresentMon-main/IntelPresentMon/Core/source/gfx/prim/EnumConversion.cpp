// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "EnumConversion.h"
#include <Core/source/infra/Logging.h>
#include <format>


namespace p2c::gfx::prim
{
	DWRITE_FONT_WEIGHT ConvertWeight(Weight w)
	{
		using enum Weight;
		switch (w)
		{
		case Normal: return DWRITE_FONT_WEIGHT_NORMAL;
		case Bold: return DWRITE_FONT_WEIGHT_BOLD;
		case Light: return DWRITE_FONT_WEIGHT_LIGHT;
		case Medium: return DWRITE_FONT_WEIGHT_MEDIUM;
		case ExtraBold: return DWRITE_FONT_WEIGHT_EXTRA_BOLD;
		case UltraLight: return DWRITE_FONT_WEIGHT_ULTRA_LIGHT;
		default:
			pmlog_warn(std::format("Bad weight: {}", (int)w));
			return DWRITE_FONT_WEIGHT_NORMAL;
		}
	}

	DWRITE_FONT_STYLE ConvertStyle(Style s)
	{
		using enum Style;
		switch (s)
		{
		case Normal: return DWRITE_FONT_STYLE_NORMAL;
		case Oblique: return DWRITE_FONT_STYLE_OBLIQUE;
		case Italic: return DWRITE_FONT_STYLE_ITALIC;
		default:
			pmlog_warn(std::format("Bad style: {}", (int)s));
			return DWRITE_FONT_STYLE_NORMAL;
		}
	}

	DWRITE_PARAGRAPH_ALIGNMENT ConvertAlignment(Alignment a)
	{
		using enum Alignment;
		switch (a)
		{
		case Center: return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		case Top: return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		case Bottom: return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		default:
			pmlog_warn(std::format("Bad algnment: {}", (int)a));
			return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		}
	}

	DWRITE_TEXT_ALIGNMENT ConvertJustification(Justification j)
	{
		using enum Justification;
		switch (j)
		{
		case Center: return DWRITE_TEXT_ALIGNMENT_CENTER;
		case Left: return DWRITE_TEXT_ALIGNMENT_LEADING;
		case Right: return DWRITE_TEXT_ALIGNMENT_TRAILING;
		case Full: return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
		default:
			pmlog_warn(std::format("Bad justification: {}", (int)j));
			return DWRITE_TEXT_ALIGNMENT_LEADING;
		}
	}
}