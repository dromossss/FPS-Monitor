// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <Core/source/pmon/metric/MetricFetcher.h>
#include <Core/source/gfx/layout/GraphData.h>
#include <memory>

namespace p2c::kern
{
	struct DataFetchPack
	{
		// functions
		void Populate(double timestamp)
		{
			if (graphData) {
				graphData->Push({ gfx::lay::DataPoint{.value = pFetcher->ReadValue(), .time = timestamp} });
				graphData->Trim(timestamp);
			}
			if (textData) {
				*textData = pFetcher->ReadStringValue();
			}
		}
		
		// data
		std::shared_ptr<pmon::met::MetricFetcher> pFetcher;
		std::shared_ptr<gfx::lay::GraphData> graphData;
		std::shared_ptr<std::wstring> textData;
	};
}