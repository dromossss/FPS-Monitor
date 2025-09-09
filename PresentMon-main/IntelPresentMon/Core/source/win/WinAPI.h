// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <CommonUtilities/win/WinAPI.h>
// remove annoying A/W macros as necessary
#undef FormatMessage
#include <Core/source/gfx/base/Geometry.h>


namespace p2c::win
{
    gfx::RectI GetWindowClientRect(HWND hWnd);
    std::wstring GetWindowTitle(HWND hWnd);
}