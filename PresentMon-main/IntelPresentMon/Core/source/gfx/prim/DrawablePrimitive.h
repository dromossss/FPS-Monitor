// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "RetainedPrimitive.h"
#include "BrushPrimitive.h"
#include "../base/Geometry.h"
#include "ForwardInterfaces.h"

namespace p2c::gfx::prim
{
	class DrawablePrimitive : public RetainedPrimitive
	{
	public:
		// not taking in pos; encoding pos in primitives
		// this is done because primitives cannot be trivially relocated, and currently there is no need for
		// hyper-dynamic reflowing that would warrant such an additional dev/proc cost
		virtual void Draw(Graphics& gfx) const = 0;
	protected:
		static ComPtr<ID2D1Brush> Brush(const BrushPrimitive& brushPrim)
		{
			return brushPrim.GetInterface();
		}
	};
}