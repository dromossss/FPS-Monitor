#pragma once
#include "../../../PresentMonAPI2/PresentMonAPI.h"

// enum annotation (enum_name_fragment, key_name_fragment, name, short_name, description)
#define ENUM_KEY_LIST_METRIC_TYPE(X_) \
		X_(METRIC_TYPE, DYNAMIC, "Dynamic Metric", "", "Metric that changes over time and requires polling using a registered query") \
		X_(METRIC_TYPE, STATIC, "Static Metric", "", "Metric that never changes and can be polled without registering a query") \
		X_(METRIC_TYPE, FRAME_EVENT, "Frame Event Metric", "", "Metric that is not polled, but rather consumed from a queue of frame events") \
		X_(METRIC_TYPE, DYNAMIC_FRAME, "Dynamic and Frame Event Metric", "", "Metric that can either be polled with statisics, or consumed from frame event queue")
