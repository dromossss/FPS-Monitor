#pragma once
#include <Core/source/win/WinAPI.h>
#define FormatMessage FormatMessageW
#define _WIN32_DCOM
#include <comdef.h>
#undef FormatMessage