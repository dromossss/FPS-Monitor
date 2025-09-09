#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_DEVICE_TYPE(X_) \
		X_(DEVICE_TYPE, INDEPENDENT, "Device Independent", "", "This device type is used for special device ID 0 which is reserved for metrics independent of any specific hardware device (e.g. FPS metrics)") \
		X_(DEVICE_TYPE, GRAPHICS_ADAPTER, "Graphics Adapter", "", "Graphics adapter or GPU device")