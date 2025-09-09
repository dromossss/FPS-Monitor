#pragma once
#include "WinAPI.h"
#include <string>

namespace pmon::util::win
{
    struct Process
    {
        // data
        DWORD pid = 0;
        DWORD parentId = 0;
        HWND hWnd = nullptr;
        std::wstring name;
        // functions
        bool operator <(const Process& rhs) const { return pid < rhs.pid; }
        bool operator==(const Process& rhs) const { return pid == rhs.pid; }
    };
}