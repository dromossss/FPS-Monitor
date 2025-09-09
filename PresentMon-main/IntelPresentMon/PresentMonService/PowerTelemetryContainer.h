// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../ControlLib/PowerTelemetryProviderFactory.h"

class PowerTelemetryContainer {
 public:
  const std::vector<std::shared_ptr<pwr::PowerTelemetryAdapter>>&
  GetPowerTelemetryAdapters() {
    return telemetry_adapters_;
  }
  bool Repopulate();
 private:
  std::vector<std::unique_ptr<pwr::PowerTelemetryProvider>> telemetry_providers_;
  std::vector<std::shared_ptr<pwr::PowerTelemetryAdapter>> telemetry_adapters_;
};