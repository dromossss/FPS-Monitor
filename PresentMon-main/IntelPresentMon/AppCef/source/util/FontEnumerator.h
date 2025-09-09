// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>
#include <string>

namespace p2c::client::util
{
	class FontEnumerator
	{
	public:
		FontEnumerator();
		const std::vector<std::wstring>& GetNames() const;
	private:
		std::vector<std::wstring> names;
	};
}