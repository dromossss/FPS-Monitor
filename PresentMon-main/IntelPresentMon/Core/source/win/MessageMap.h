// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "WinAPI.h"
#include <unordered_map>

namespace p2c::win
{
    class MessageMap
    {
    public:
        MessageMap();
        const wchar_t* GetMessageName(DWORD msg) const;
    private:
        std::unordered_map<DWORD, const wchar_t*> map;
    };
}
