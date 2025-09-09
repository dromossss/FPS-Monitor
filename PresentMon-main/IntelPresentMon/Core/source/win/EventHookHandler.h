// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "WinAPI.h"


namespace p2c::win
{
    class EventHookHandler
    {
        friend class EventHookManager;
    public:
        struct Filter
        {
            DWORD minEvent = EVENT_MIN;
            DWORD maxEvent = EVENT_MAX;
            DWORD pid = 0;
            DWORD tid = 0;
        };
        virtual Filter GetFilter() const = 0;
        virtual ~EventHookHandler() = default;
    protected:
        virtual void Handle(
            HWINEVENTHOOK hook, DWORD event, HWND hWnd,
            LONG idObject, LONG idChild,
            DWORD dwEventThread, DWORD dwmsEventTime) = 0;
    };
}