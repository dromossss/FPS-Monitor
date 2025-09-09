// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../FrameDataStream.h"
#include <optional>
#include <vector>
#include <string>
#include <unordered_set>

namespace p2c::cli::cons
{
    class ConsoleWaitControl;
}

namespace p2c::cli::pmon::stream
{
    class LoggedFrameDataState
    {
    public:
        LoggedFrameDataState(std::string filePath, cons::ConsoleWaitControl* pWaitControl);
        std::vector<FrameDataStream::Struct> Pull(double timestamp, uint32_t pid);
    private:
        static constexpr size_t initialCacheSize_ = 120;
        std::optional<double> cacheTimestamp_;
        std::vector<FrameDataStream::Struct> cache_;
        cons::ConsoleWaitControl* pWaitControl_;
        std::unordered_set<uint32_t> pids_;
    };
}