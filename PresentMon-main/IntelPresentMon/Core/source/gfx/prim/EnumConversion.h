// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Enums.h"
#include <dwrite.h>


namespace p2c::gfx::prim
{
	DWRITE_FONT_WEIGHT ConvertWeight(Weight w);

	DWRITE_FONT_STYLE ConvertStyle(Style s);

	DWRITE_PARAGRAPH_ALIGNMENT ConvertAlignment(Alignment a);

	DWRITE_TEXT_ALIGNMENT ConvertJustification(Justification j);
}