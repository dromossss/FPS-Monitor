// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "FrameDemultiplexer.h"
#include <PresentMonAPI/PresentMonAPI.h>
#include <format>
#include <filesystem>
#include "MakeCsvName.h"

namespace p2c::cli::dat
{
	FrameDemultiplexer::FrameDemultiplexer(std::vector<std::string> groups, std::optional<std::string> customFileName)
		:
		customFileName_{ std::move(customFileName) },
		groups_{ std::move(groups) }
	{}
	void FrameDemultiplexer::Process(const PM_FRAME_DATA& frame)
	{
		if (auto i = procs_.find(frame.process_id); i != procs_.end()) {
			i->second->Process(frame);
		}
		else {
			auto processName = std::filesystem::path{ frame.application }.filename().string();
			auto fileName = MakeCsvName(false, frame.process_id, std::move(processName), customFileName_);
			auto pWriter = std::make_shared<CsvWriter>(std::move(fileName), groups_);
			pWriter->Process(frame);
			procs_.emplace(frame.process_id, std::move(pWriter));
		}
	}
}