// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "FrameSink.h"
#include "CsvWriter.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace p2c::cli::dat
{
	// TODO: add ability to configure file names better
	class FrameDemultiplexer : public FrameSink
	{
	public:
		FrameDemultiplexer(std::vector<std::string> groups, std::optional<std::string> customFileName);
		void Process(const PM_FRAME_DATA& frame) override;
	private:
		std::unordered_map<uint32_t, std::shared_ptr<CsvWriter>> procs_;
		std::vector<std::string> groups_;
		std::optional<std::string> customFileName_;
	};
}