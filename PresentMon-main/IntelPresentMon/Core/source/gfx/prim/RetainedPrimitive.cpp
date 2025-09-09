// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "RetainedPrimitive.h"
#include "../Graphics.h"
#include <Core/source/win/WinAPI.h>
#include <d2d1_3.h>

namespace p2c::gfx::prim
{
	ID2D1DeviceContext2& RetainedPrimitive::Context(Graphics& gfx)
	{
		return *gfx.pContext2d;
	}

	ID2D1Factory3& RetainedPrimitive::Factory(Graphics& gfx)
	{
		return *gfx.pFactory2d;
	}

	IDWriteFactory& RetainedPrimitive::Write(Graphics& gfx)
	{
		return *gfx.pWriteFactory;
	}
}