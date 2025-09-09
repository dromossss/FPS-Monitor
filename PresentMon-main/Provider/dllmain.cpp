// Copyright (C) 2017-2024 Intel Corporation
// SPDX-License-Identifier: MIT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// This DLL is just used to store the Intel-PresentMon ETW Provider resources
BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID)
{
    return TRUE;
}

