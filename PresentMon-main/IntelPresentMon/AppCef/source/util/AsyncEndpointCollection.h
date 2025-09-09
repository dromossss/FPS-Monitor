// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "AsyncEndpoint.h"
#include <unordered_map>
#include <memory>

namespace p2c::client::util
{
	class AsyncEndpointCollection
	{
	public:
		AsyncEndpointCollection();
		const AsyncEndpoint* Find(const std::string& key) const;
	private:
		// functions
		template<class T>
		void AddEndpoint();
		// data
		std::unordered_map<std::string, std::unique_ptr<AsyncEndpoint>> endpoints;
	};
}