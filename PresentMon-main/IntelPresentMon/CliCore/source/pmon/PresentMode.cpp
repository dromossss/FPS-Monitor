// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#define KEEP_PM_PRESENT_MODE_X_LIST
#include "PresentMode.h"

namespace p2c::cli::pmon
{
	PresentMode ConvertPresentMode(PM_PRESENT_MODE mode)
	{
		return PresentMode(mode);
	}

	std::wstring PresentModeToString(PresentMode mode)
	{
		switch (mode)
		{
#define X_(a, b, c) case PresentMode::b: return c;
			PM_PRESENT_MODE_X_LIST
#undef X_
		default: return L"Invalid";
		}
	}
}