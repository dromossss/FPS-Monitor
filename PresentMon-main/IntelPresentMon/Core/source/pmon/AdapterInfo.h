// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <string>

namespace p2c::pmon
{
	struct AdapterInfo
	{
		uint32_t id;
		std::string vendor;
		std::string name;
	};
}