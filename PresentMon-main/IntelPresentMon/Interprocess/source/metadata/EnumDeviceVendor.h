#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_DEVICE_VENDOR(X_) \
		X_(DEVICE_VENDOR, INTEL, "Intel", "", "Device vendor Intel") \
		X_(DEVICE_VENDOR, NVIDIA, "NVIDIA", "", "Device vendor NVIDIA") \
		X_(DEVICE_VENDOR, AMD, "AMD", "", "Device vendor AMD") \
		X_(DEVICE_VENDOR, UNKNOWN, "Unknown", "", "Unknown device vendor")