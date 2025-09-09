// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "WinAPI.h"
#include <memory>
#include <Core/source/infra/Logging.h>
#include "EventHookHandler.h"
#include <mutex>


namespace p2c::win
{
    // TODO: consider multi-thread implications of this singleton
    class EventHookManager
    {
    public:
        class [[nodiscard]] Token
        {
        public:
            Token(HWINEVENTHOOK hHook);
            Token(Token&& donor) noexcept;
            Token& operator=(Token&& donor) noexcept;
            ~Token();
        private:
            HWINEVENTHOOK hHook = nullptr;
        };
        static [[nodiscard]] Token AddHandler(std::shared_ptr<EventHookHandler> pHandler);
    private:
        HWINEVENTHOOK AddHandlerInternal(std::shared_ptr<EventHookHandler> pHandler);
        static void CALLBACK RouteWinEvent(
            HWINEVENTHOOK hook, DWORD event, HWND hWnd,
            LONG idObject, LONG idChild,
            DWORD dwEventThread, DWORD dwmsEventTime);
        static EventHookManager& Get();
        void RemoveHandler(HWINEVENTHOOK hHook);
        ~EventHookManager();
        std::unordered_map<HWINEVENTHOOK, std::shared_ptr<EventHookHandler>> handlerMap;
    };
}
