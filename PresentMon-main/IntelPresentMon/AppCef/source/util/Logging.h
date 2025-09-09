#pragma once
//#define PMLOG_BUILD_LEVEL Verbose
//#define VVV_V8ASYNC
#include <CommonUtilities/log/Log.h>


namespace p2c::v
{
#ifndef VVV_V8ASYNC // system for async calls and signals between V8 and C++
	inline constexpr bool v8async = false;
#else
	inline constexpr bool v8async = true;
#endif
#ifndef VVV_HOTKEY // system that processes raw keyboard input
	inline constexpr bool hotkey = false;
#else
	inline constexpr bool hotkey = true;
#endif	
}