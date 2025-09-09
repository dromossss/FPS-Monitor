// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "KernelWindow.h"

namespace p2c::win 
{
    class OverlayWindow : public KernelWindow
    {
    public:
        OverlayWindow(bool fsMode, int x, int y, gfx::DimensionsI clientDimensions, std::wstring name);
        std::optional<LRESULT> CustomHandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
        bool Fullscreen() const override;
        bool Standard() const override;
    private:
        bool fullscreenMode;
    };
} 
