#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_PRESENT_MODE(X_) \
		X_(PRESENT_MODE, HARDWARE_LEGACY_FLIP, "Hardware: Legacy Flip", "", "Legacy flip present with direct control of hardware scanout frame buffer (fullscreen exclusive)") \
		X_(PRESENT_MODE, HARDWARE_LEGACY_COPY_TO_FRONT_BUFFER, "Hardware: Legacy Copy to Front Buffer", "", "Legacy bitblt model present copying from backbuffer to scanout frame buffer (fullscreen exclusive)") \
		X_(PRESENT_MODE, HARDWARE_INDEPENDENT_FLIP, "Hardware: Independent Flip", "", "Application window client area covers display output and its swap chain is being used directly for scanout without DWM intervention") \
		X_(PRESENT_MODE, COMPOSED_FLIP, "Composed: Flip", "", "Application front buffer is being composed by DWM to final scanout frame buffer") \
		X_(PRESENT_MODE, HARDWARE_COMPOSED_INDEPENDENT_FLIP, "Hardware Composed: Independent Flip", "", "Application is able to directly write into final scanout frame buffer to compose itself without DWM intervention") \
		X_(PRESENT_MODE, COMPOSED_COPY_WITH_GPU_GDI, "Composed: Copy with GPU GDI", "", "GDI bitblt composition using the GPU") \
		X_(PRESENT_MODE, COMPOSED_COPY_WITH_CPU_GDI, "Composed: Copy with CPU GDI", "", "GDI bitblt composition using the CPU") \
		X_(PRESENT_MODE, UNKNOWN, "Unknown", "", "Unknown present mode")