#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_GRAPHICS_RUNTIME(X_) \
		X_(GRAPHICS_RUNTIME, UNKNOWN, "Unknown", "", "Unknown graphics runtime") \
		X_(GRAPHICS_RUNTIME, DXGI, "DXGI", "", "DirectX Graphics Infrastructure runtime") \
		X_(GRAPHICS_RUNTIME, D3D9, "Direct3D 9", "", "Direct3D 9 runtime")