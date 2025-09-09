// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <variant>
#include <string>
#include <map>

namespace p2c::gfx::lay::sty::at
{
	using RawString = std::wstring;
	using RawNumber = double;
	using RawBool = bool;
	using RawAttributeSimpleValue = std::variant<RawString, RawNumber, RawBool>;
	using RawAttributeObjectValue = std::map<std::string, RawAttributeSimpleValue>;
	using RawAttributeValue = std::variant<
		RawString,
		RawNumber,
		RawBool,
		RawAttributeObjectValue
	>;
}