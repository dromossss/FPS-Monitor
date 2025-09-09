// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "LoggedFrameDataStream.h"
#include <format>
#include <Core/source/infra/log/Logging.h>
#include <Core/source/infra/util/Assert.h>

namespace p2c::cli::pmon::stream
{
    LoggedFrameDataStream::LoggedFrameDataStream(uint32_t pid, std::shared_ptr<LoggedFrameDataState> pState)
        :
        pid_(pid),
        cache_(initialCacheSize_),
        pState_{ std::move(pState) }
    {}

    std::span<const FrameDataStream::Struct> LoggedFrameDataStream::Pull(double timestamp)
    {
        if (!cacheTimestamp_ || cacheTimestamp_ != timestamp)
        {
            cache_ = pState_->Pull(timestamp, pid_);
            cacheTimestamp_ = timestamp;
        }
        return cache_;
    }
    uint32_t LoggedFrameDataStream::GetPid() const
    {
        return pid_;
    }
}