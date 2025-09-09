// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once

namespace p2c::gfx::lay::sty::at
{
	namespace Special
	{
		struct Base {};
		struct Inherit : Base { static constexpr const wchar_t* key = L"@Inherit"; };
		struct Auto : Base { static constexpr const wchar_t* key = L"@Auto"; };
	}
}