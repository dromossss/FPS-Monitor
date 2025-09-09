#pragma once

//#define PMLOG_BUILD_LEVEL Verbose
//#define VVV_ETWQ
#include "../CommonUtilities/log/Log.h"


namespace pmon::svc::v
{
#ifndef VVV_ETWQ // system that handles the dequeing distribution of frame event data
	inline constexpr bool etwq = false;
#else
	inline constexpr bool etwq = true;
#endif
}