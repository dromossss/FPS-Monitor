// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "MessageBox.h"
#include "Window.h"

namespace p2c::win
{
	void OpenMessageBox(const std::wstring& title, const std::wstring& body, Window* pWindow)
	{
		MessageBoxW(pWindow ? pWindow->GetHandle() : nullptr, body.c_str(), title.c_str(), MB_ICONERROR);
	}
}