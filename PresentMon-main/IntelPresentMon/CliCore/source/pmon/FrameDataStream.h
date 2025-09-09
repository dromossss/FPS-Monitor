// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <PresentMonAPI/PresentMonAPI.h>
#include <span>

namespace p2c::cli::pmon
{
    class FrameDataStream
    {
    public:
        using Struct = PM_FRAME_DATA;
        virtual std::span<const Struct> Pull(double timestamp) = 0;
        virtual uint32_t GetPid() const = 0;
        virtual ~FrameDataStream() = default;
    };
}