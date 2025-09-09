// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "PresentMonPowerTelemetry.h"
#include "PowerTelemetryAdapter.h"

namespace pwr
{
    class PowerTelemetryProvider
    {
    public:
        virtual ~PowerTelemetryProvider() = default;
        virtual const std::vector<std::shared_ptr<PowerTelemetryAdapter>>& GetAdapters() noexcept = 0;
        virtual uint32_t GetAdapterCount() const noexcept = 0;
    };
}