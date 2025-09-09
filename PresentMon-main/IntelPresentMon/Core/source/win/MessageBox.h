// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <string>

namespace p2c::win
{
	class Window;

	void OpenMessageBox(const std::wstring& title, const std::wstring& body, Window* pWindow = nullptr);
}
