#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

#define ENUM_KEY_LIST_ENUM(X_) \
		X_(ENUM, STATUS, "Statuses", "", "List of all status/error codes returned by API functions") \
		X_(ENUM, METRIC, "Metrics", "", "List of all metrics that are pollable via the API") \
		X_(ENUM, METRIC_TYPE, "Metric Types", "", "List of all metric types (dynamic polled, static, etc.)") \
		X_(ENUM, DEVICE_VENDOR, "Vendors", "", "List of all known hardware vendors (IHVs) for GPUs and other hardware") \
		X_(ENUM, PRESENT_MODE, "Present Modes", "", "List of all known modes of frame presentation") \
		X_(ENUM, UNIT, "Units", "", "List of all units of measure used for metrics") \
		X_(ENUM, STAT, "Statistics", "", "List of all statistical variations of the data (average, 99th percentile, etc.)") \
		X_(ENUM, DATA_TYPE, "Data Types", "", "List of all C++ language data types used for metrics") \
		X_(ENUM, GRAPHICS_RUNTIME, "Graphics Runtime", "", "Graphics runtime subsystem used to make the present call") \
		X_(ENUM, FRAME_TYPE, "Frame Type", "", "Type of frame rendered") \
		X_(ENUM, DEVICE_TYPE, "Device Type", "", "Type of device in the list of devices associated with metrics") \
		X_(ENUM, METRIC_AVAILABILITY, "Metric Availability", "", "Availability status of a metric with respect to a given device") \
		X_(ENUM, NULL_ENUM, "Null Enum", "", "Used to indicate an empty / invalid enum type")
