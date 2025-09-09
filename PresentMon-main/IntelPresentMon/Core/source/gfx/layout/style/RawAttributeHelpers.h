// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "RawAttributeValue.h"
#include "../EnumUtils.h"
#include <Core/source/gfx/base/Geometry.h>

namespace p2c::gfx::lay::sty::at::make
{
	RawAttributeObjectValue Color(gfx::Color c);

	template<typename S>
	RawString Special()
	{
		return std::wstring{ S::key };
	}

	template<typename E>
	RawString Enum(E key)
	{
		return L"*" + EnumRegistry<E>::FromEnum(key);
	}
}