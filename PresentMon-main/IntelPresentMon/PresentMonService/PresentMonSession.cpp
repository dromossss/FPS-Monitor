// Copyright (C) 2022-2023 Intel Corporation
// SPDX-License-Identifier: MIT
#include "PresentMonSession.h"

void PresentMonSession::SetCpu(const std::shared_ptr<pwr::cpu::CpuTelemetry>& pCpu) {
    cpu_ = pCpu.get();
}

std::vector<std::shared_ptr<pwr::PowerTelemetryAdapter>> PresentMonSession::EnumerateAdapters() {
    if (telemetry_container_) {
        return telemetry_container_->GetPowerTelemetryAdapters();
    }
    else {
        return {};
    }
}

std::string PresentMonSession::GetCpuName() {
    if (cpu_) {
        return cpu_->GetCpuName();
    }
    else {
        return std::string{ "UNKOWN_CPU" };
    }
}

double PresentMonSession::GetCpuPowerLimit() {
    if (cpu_) {
        return cpu_->GetCpuPowerLimit();
    }
    else {
        return 0.;
    }
}

PM_STATUS PresentMonSession::SelectAdapter(uint32_t adapter_id) {
    if (telemetry_container_) {
        if (adapter_id > telemetry_container_->GetPowerTelemetryAdapters().size()) {
            return PM_STATUS_INVALID_ADAPTER_ID;
        }
        current_telemetry_adapter_id_ = adapter_id;
    }
    return PM_STATUS::PM_STATUS_SUCCESS;
}

// TODO: copied from legacy api header
// find a better home for these defines, and how to communicate them to app devs
#define MIN_PM_TELEMETRY_PERIOD 1
#define MAX_PM_TELEMETRY_PERIOD 1000

PM_STATUS PresentMonSession::SetGpuTelemetryPeriod(uint32_t period_ms) {
    if (period_ms < MIN_PM_TELEMETRY_PERIOD ||
        period_ms > MAX_PM_TELEMETRY_PERIOD) {
        return PM_STATUS::PM_STATUS_OUT_OF_RANGE;
    }
    gpu_telemetry_period_ms_ = period_ms;
    return PM_STATUS_SUCCESS;
}

uint32_t PresentMonSession::GetGpuTelemetryPeriod() {
    return gpu_telemetry_period_ms_;
}

PM_STATUS PresentMonSession::SetEtwFlushPeriod(std::optional<uint32_t> periodMs)
{
    if (periodMs && (*periodMs < 1 || *periodMs > 1000)) {
        return PM_STATUS::PM_STATUS_OUT_OF_RANGE;
    }
    etw_flush_period_ms_ = periodMs;
    return PM_STATUS_SUCCESS;
}

std::optional<uint32_t> PresentMonSession::GetEtwFlushPeriod()
{
    return etw_flush_period_ms_;
}

int PresentMonSession::GetActiveStreams() {
    return streamer_.NumActiveStreams();
}

void PresentMonSession::SetPowerTelemetryContainer(PowerTelemetryContainer* ptc) {
    telemetry_container_ = ptc;
}