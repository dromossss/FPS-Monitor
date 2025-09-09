#pragma once
#include "IpcInvocationManager.h"


namespace p2c::client::util
{
	std::unordered_map<std::string, std::function<void(IpcInvocationManager::CallbackContext, CefRefPtr<CefV8Value>, CefClient&)>>
		IpcInvocationManager::dispatchBindings_;
}