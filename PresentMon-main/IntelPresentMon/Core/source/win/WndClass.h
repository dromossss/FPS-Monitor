// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "WinAPI.h"

namespace p2c::win
{
    class WndClass
    {
    public:
        WndClass();
        WndClass(const WndClass&) = delete;
        WndClass& operator=(const WndClass&) = delete;
        ~WndClass();
        ATOM GetAtom() const;
    private:
        // callbacks
        static LRESULT WINAPI SetupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT WINAPI ThunkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        // data
        ATOM atom;
    };
}
