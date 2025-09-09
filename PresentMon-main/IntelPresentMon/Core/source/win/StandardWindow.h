// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "KernelWindow.h"
#include <memory>

namespace p2c::gfx
{
    class Graphics;
}

namespace p2c::win 
{
    class StandardWindow : public KernelWindow
    {
    public:
        StandardWindow(int x, int y, gfx::DimensionsI clientDimensions, std::wstring name, bool bringToFront = true);
        ~StandardWindow() override;
        std::optional<LRESULT> CustomHandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
        bool Standard() const override;
        bool Fullscreen() const override;
    };
} 
