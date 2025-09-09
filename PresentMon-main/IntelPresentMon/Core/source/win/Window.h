// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "WinAPI.h"
#include <string>
#include <optional>
#include <Core/source/gfx/base/Geometry.h>

namespace p2c::win
{
    gfx::DimensionsI RectToDims(const RECT& r);
    gfx::RectI RectToRectI(const RECT& r);

    class Window
    {
        friend class WndClass;
    public:
        Window(std::wstring title, DWORD styles);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        virtual ~Window();
        HWND GetHandle();
        const std::wstring& GetTitle() const;
        gfx::Vec2I GetPosition() const;
        void Move(gfx::Vec2I pos);
        void Reorder(HWND base);
        void ReorderBehind(HWND base);
        void SetTopmost();
        void ClearTopmost();
        void Close();
        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
        void SetMessageLogging(bool enable);
        void Hide();
        void Show();
        void Resize(gfx::DimensionsI size);
    protected:
        gfx::DimensionsI ComputeWindowDimensions(gfx::DimensionsI clientArea);
        static ATOM GetDefaultClass();
        virtual std::optional<LRESULT> CustomHandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
        DWORD styles;
    private:
        // functions
        // SetHandle should only ever be called by initial setup thunk in WndClass
        // thereby "completing" the construction of a Window
        void SetHandle(HWND hWnd_);
        void LogMessage(DWORD msg, LPARAM, WPARAM);
        static const wchar_t* LookupMessageName(DWORD msg);
        // data
        HWND hWnd = nullptr;
        std::wstring title;
        bool loggingMessages = false;
    };
}
