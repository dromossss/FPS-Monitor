// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../FrameDataStream.h"
#include "LoggedFrameDataState.h"
#include <string>
#include <optional>
#include <span>
#include <vector>
#include <memory>

namespace p2c::cli::pmon
{
    class ProcessStream;
}

namespace p2c::cli::pmon::stream
{
    class LoggedFrameDataStream : public FrameDataStream
    {
    public:
        LoggedFrameDataStream(uint32_t pid, std::shared_ptr<LoggedFrameDataState> pState);
        std::span<const Struct> Pull(double timestamp) override;
        uint32_t GetPid() const override;
    private:
        static constexpr size_t initialCacheSize_ = 120;
        uint32_t pid_;
        std::shared_ptr<LoggedFrameDataState> pState_;
        std::optional<double> cacheTimestamp_;
        std::vector<Struct> cache_;
    };
}