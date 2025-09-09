// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Window.h"

namespace p2c::win
{
    class KernelWindow : public Window
    {
    public:
        using Window::Window;
        virtual bool Fullscreen() const = 0;
        virtual bool Standard() const = 0;
    };
}
