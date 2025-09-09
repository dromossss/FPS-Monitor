// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <CommonUtilities/win/WinAPI.h>
#include <include/cef_v8.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <future>
#include <unordered_map>
#include "AsyncEndpointCollection.h"


namespace p2c::client::util
{
	class AsyncEndpointManager
	{
	public:
		// constants
		static constexpr std::string GetDispatchMessageName() { return "disp-async"; }
		static constexpr std::string GetResolveMessageName() { return "reso-async"; }
		// types
		struct CallbackContext
		{
			CefRefPtr<CefV8Value> pV8ResolveFunction;
			CefRefPtr<CefV8Value> pV8RejectFunction;
			CefRefPtr<CefV8Context> pV8Context;
		};
		struct Invocation
		{
			CallbackContext context;
			const AsyncEndpoint* pEndpoint;
			std::future<void> taskFuture;
		};
		// functions
		void DispatchInvocation(const std::string& key, CallbackContext ctx, CefRefPtr<CefV8Value> pObj, CefBrowser& browser, cef::DataBindAccessor& accessor);
		void ResolveInvocation(uint64_t uid, bool success, CefRefPtr<CefValue> pArgs);
	private:
		uint64_t nextUid = 0;
		AsyncEndpointCollection endpoints;
		// todo: use separate mutex for endpoints+uid than for invocations
		std::mutex mtx;
		std::unordered_map<uint64_t, Invocation> invocations;
	};
}

#undef KEY_LIST