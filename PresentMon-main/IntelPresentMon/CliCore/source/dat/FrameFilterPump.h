// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "FrameSink.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

namespace p2c::cli::pmon
{
	class FrameDataStream;
}

namespace p2c::cli::dat
{
	class FrameFilterPump
	{
	public:
		FrameFilterPump(
			std::shared_ptr<pmon::FrameDataStream> pSource,
			std::shared_ptr<FrameSink> pSink,
			const std::vector<std::string>& excludes,
			const std::vector<uint32_t>& includes,
			bool excludeDropped,
			bool ignoreCase);
		void AddInclude(uint32_t pid);
		void Process(double timestamp);
		uint32_t GetPid() const;
	private:
		std::shared_ptr<pmon::FrameDataStream> pSource_;
		std::shared_ptr<FrameSink> pSink_;
		// TODO: add exclude-by-pid to speed up filtering
		std::unordered_set<std::string> excludes_;
		// TODO: use includes for an early-pass in non-whitelist situations
		std::unordered_set<uint32_t> includes_;
		bool ignoreCase_;
		bool excludeDropped_;
	};
}