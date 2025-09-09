// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../FrameDataStream.h"
#include <string>
#include <optional>
#include <span>
#include <vector>

namespace p2c::cli::pmon
{
    class ProcessStream;
}

namespace p2c::cli::pmon::stream
{
    class LiveFrameDataStream : public FrameDataStream
    {
    public:
        LiveFrameDataStream(uint32_t pid);
        std::span<const Struct> Pull(double timestamp) override;
        uint32_t GetPid() const override;
        LiveFrameDataStream(const LiveFrameDataStream&) = delete;
        LiveFrameDataStream& operator=(const LiveFrameDataStream&) = delete;
        ~LiveFrameDataStream() override;
    private:
        static constexpr size_t initialCacheSize_ = 120;
        uint32_t pid_;
        std::optional<double> cacheTimestamp_;
        std::vector<Struct> cache_;
    };
}