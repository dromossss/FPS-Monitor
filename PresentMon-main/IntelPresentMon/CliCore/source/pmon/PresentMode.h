// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <string>
#include <PresentMonAPI/PresentMonAPI.h>

#define PM_PRESENT_MODE_X_LIST \
	X_(PM_PRESENT_MODE_HARDWARE_LEGACY_FLIP, HardwareLegacyFlip, L"HW Legacy Flip") \
	X_(PM_PRESENT_MODE_HARDWARE_LEGACY_COPY_TO_FRONT_BUFFER, HardwareLegacyCopyToFrontBuffer, L"HW Legacy Copy To Front Buffer") \
	X_(PM_PRESENT_MODE_HARDWARE_INDEPENDENT_FLIP, HardwareIndependentFlip, L"HW Independent Flip") \
	X_(PM_PRESENT_MODE_COMPOSED_FLIP, ComposedFlip, L"Composed Flip") \
	X_(PM_PRESENT_MODE_HARDWARE_COMPOSED_INDEPENDENT_FLIP, HardwareComposedIndependentFlip, L"HW Composed Independent Flip") \
	X_(PM_PRESENT_MODE_COMPOSED_COPY_WITH_GPU_GDI, ComposedCopyWithGpuGdi, L"Composed Copy With GPU GDI") \
	X_(PM_PRESENT_MODE_COMPOSED_COPY_WITH_CPU_GDI, ComposedCopyWithCpuGdi, L"Composed Copy With CPU GDI") \
	X_(PM_PRESENT_MODE_UNKNOWN, Unknown, L"Unknown")

namespace p2c::cli::pmon
{
	enum class PresentMode : uint32_t
	{
#define X_(a, b, c) b = uint32_t(a),
		PM_PRESENT_MODE_X_LIST
#undef X_
	};

	PresentMode ConvertPresentMode(PM_PRESENT_MODE mode);
	std::wstring PresentModeToString(PresentMode mode);
}

#ifndef KEEP_PM_PRESENT_MODE_X_LIST
#undef PM_PRESENT_MODE_X_LIST
#endif