// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <Core/source/gfx/base/ComPtr.h>
#include "ComManager.h"

struct IWbemServices;

namespace p2c::win::com
{
	class WbemListener;
	class WbemSink;

	class WbemConnection
	{
	public:
		WbemConnection();
		template<class T, class...P>
		std::unique_ptr<WbemListener> MakeListener(P&&...params)
		{
			auto pSink = new T{ std::forward<P>(params)... };
			pSink->AddRef();
			return CreateListener_(pSink);
		}
	private:
		// function
		std::unique_ptr<WbemListener> CreateListener_(WbemSink* pSink);
		// data
		ComManager com;
		Microsoft::WRL::ComPtr<IWbemServices> pConnection;
	};
}