// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <Core/source/win/EventHookHandler.h>
#include <Core/source/win/Process.h>

namespace p2c::kern
{
    class Overlay;

    class WindowActivateHandler : public win::EventHookHandler
    {
    public:
        WindowActivateHandler(win::Process proc, Overlay* pOverlay);
        win::EventHookHandler::Filter GetFilter() const override;
    protected:
        void Handle(
            HWINEVENTHOOK hook, DWORD event, HWND hWnd,
            LONG idObject, LONG idChild,
            DWORD dwEventThread, DWORD dwmsEventTime) override;
    private:
        win::Process proc;
        Overlay* pOverlay;
    };
}