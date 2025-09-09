// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "ForwardInterfaces.h"


namespace p2c::gfx
{
	class Graphics;
}

namespace p2c::gfx::prim
{
	class RetainedPrimitive
	{
	public:
		virtual ~RetainedPrimitive() = default;
	protected:
		// expose access to subset of private members to all RetainedPrimitives
		static ID2D1DeviceContext2& Context(Graphics& gfx);
		static ID2D1Factory3& Factory(Graphics& gfx);
		static IDWriteFactory& Write(Graphics& gfx);
	};
}