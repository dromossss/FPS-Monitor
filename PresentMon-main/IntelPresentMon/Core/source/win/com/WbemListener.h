// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <Core/source/gfx/base/ComPtr.h>
#include <Wbemidl.h>
#include "WbemSink.h"


namespace p2c::win::com
{
	class WbemListener
	{
	public:
        WbemListener(WbemSink* pSink_,
			Microsoft::WRL::ComPtr<IWbemServices> pConnection_);
		WbemListener(const WbemListener&) = delete;
		WbemListener& operator=(const WbemListener&) = delete;
		~WbemListener();
    private:
		WbemSink* pSink = nullptr;
        Microsoft::WRL::ComPtr<IWbemObjectSink> pStub;
		Microsoft::WRL::ComPtr<IWbemServices> pConnection;
	};
}