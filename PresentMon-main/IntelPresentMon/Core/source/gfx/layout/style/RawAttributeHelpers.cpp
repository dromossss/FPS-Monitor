// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "RawAttributeHelpers.h"

namespace p2c::gfx::lay::sty::at::make
{
	RawAttributeObjectValue Color(gfx::Color c)
	{
// this operation works with low values; will never overflow
#pragma warning(push)
#pragma warning(disable : 26451)
		return RawAttributeObjectValue{
			{ "r", double(255.f * c.r) },
			{ "g", double(255.f * c.g) },
			{ "b", double(255.f * c.b) },
			{ "a", double(c.a) },
		};
#pragma warning(pop)
	}
}