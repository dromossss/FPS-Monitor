#pragma once
#include "Log.h"

#define LOG(LVL) pmlog_(GLOGSHIM_##LVL##_).stream()

#define GLOGSHIM_INFO_ ::pmon::util::log::Level::Info
#define GLOGSHIM_ERROR_ ::pmon::util::log::Level::Error