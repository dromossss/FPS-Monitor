// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "SignalManager.h"
#include "CefValues.h"
#include "Logging.h"
#include <CommonUtilities/str/String.h>
#include <algorithm>
#include <include/wrapper/cef_helpers.h>


namespace p2c::client::util
{
	void SignalManager::RegisterCallback(const std::string& key, CallbackContext ctx)
	{
		if (auto i = std::find(std::begin(keyList), std::end(keyList), key); i == std::end(keyList))
		{
			pmlog_warn(std::format("Key [{}] not a valid signal key", key));
		}
		callbacks[key] = std::move(ctx);
	}

	template<typename ...Ts>
	void SignalManager::ImplementSignal_(const std::string& key, Ts&& ...args)
	{
		CEF_REQUIRE_RENDERER_THREAD();

		if (auto i = callbacks.find(key); i != callbacks.end())
		{
			auto&& ctx = i->second;
			ctx.pV8Function->ExecuteFunctionWithContext(ctx.pV8Context, nullptr, BuildV8ArgumentList(args...));
		}
		else
		{
			pmlog_warn(std::format("Signal fired for [{}] with no handler set", key));
		}
	}

	// List of Signal-firing functions (call from C++ to invoke a callback registered by JS side)
	//
	// targetLost(pid:number)
	void SignalManager::SignalTargetLost(uint32_t pid)
	{
		ImplementSignal_(Keys::targetLost(), pid);
	}
	// hotkeyFired(action:number)
	void SignalManager::SignalHotkeyFired(uint32_t action)
	{
		ImplementSignal_(Keys::hotkeyFired(), action);
	}
	// presentmonInitFailed()
	void SignalManager::SignalPresentmonInitFailed()
	{
		ImplementSignal_(Keys::presentmonInitFailed());
	}
	// overlayDied()
	void SignalManager::SignalOverlayDied()
	{
		ImplementSignal_(Keys::overlayDied());
	}
	// stalePid()
	void SignalManager::SignalStalePid()
	{
		ImplementSignal_(Keys::stalePid());
	}
}