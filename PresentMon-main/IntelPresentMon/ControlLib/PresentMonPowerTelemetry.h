// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>
#include <bitset>
#include <array>

enum class PresentMonPsuType
{
    None,
    Pcie,
    Pin6,
    Pin8,
};

struct PresentMonPsuPowerTelemetryInfo {
    PresentMonPsuType psu_type;
    double psu_power;
    double psu_voltage;
};

struct PresentMonPowerTelemetryInfo {
    uint64_t qpc;

    double time_stamp;

    double gpu_power_w;
    double gpu_sustained_power_limit_w;
    double gpu_voltage_v;
    double gpu_frequency_mhz;
    double gpu_temperature_c;
    double gpu_utilization;
    double gpu_render_compute_utilization;
    double gpu_media_utilization;
    double gpu_effective_frequency_mhz;
    double gpu_voltage_regulator_temperature_c;
    double gpu_mem_effective_bandwidth_gbps;
    double gpu_overvoltage_percent;
    double gpu_temperature_percent;
    double gpu_power_percent;
    double gpu_card_power_w;

    double vram_power_w;
    double vram_voltage_v;
    double vram_frequency_mhz;
    double vram_effective_frequency_gbps;
    double vram_temperature_c;

    std::array<double, 5> fan_speed_rpm;
    std::array<int32_t, 5> max_fan_speed_rpm;
    std::array<PresentMonPsuPowerTelemetryInfo, 5> psu;

    // GPU memory state
    uint64_t gpu_mem_total_size_b;
    uint64_t gpu_mem_used_b;

    // GPU memory bandwidth
    uint64_t gpu_mem_max_bandwidth_bps;
    double gpu_mem_write_bandwidth_bps;
    double gpu_mem_read_bandwidth_bps;

    // Throttling flags
    bool gpu_power_limited;
    bool gpu_temperature_limited;
    bool gpu_current_limited;
    bool gpu_voltage_limited;
    bool gpu_utilization_limited;

    bool vram_power_limited;
    bool vram_temperature_limited;
    bool vram_current_limited;
    bool vram_voltage_limited;
    bool vram_utilization_limited;
};

enum class GpuTelemetryCapBits {
    time_stamp,
    gpu_power,
    gpu_sustained_power_limit,
    gpu_voltage,
    gpu_frequency,
    gpu_temperature,
    gpu_utilization,
    gpu_render_compute_utilization,
    gpu_media_utilization,
    vram_power,
    vram_voltage,
    vram_frequency,
    vram_effective_frequency,
    vram_temperature,
    fan_speed_0,
    fan_speed_1,
    fan_speed_2,
    fan_speed_3,
    fan_speed_4,
    psu_info_0,
    psu_info_1,
    psu_info_2,
    psu_info_3,
    psu_info_4,
    gpu_mem_size,
    gpu_mem_used,
    gpu_mem_max_bandwidth,
    gpu_mem_write_bandwidth,
    gpu_mem_read_bandwidth,
    gpu_power_limited,
    gpu_temperature_limited,
    gpu_current_limited,
    gpu_voltage_limited,
    gpu_utilization_limited,
    vram_power_limited,
    vram_temperature_limited,
    vram_current_limited,
    vram_voltage_limited,
    vram_utilization_limited,
    gpu_effective_frequency,
    gpu_voltage_regulator_temperature,
    gpu_mem_effective_bandwidth,
    gpu_overvoltage_percent,
    gpu_temperature_percent,
    gpu_power_percent,
    max_fan_speed_0,
    max_fan_speed_1,
    max_fan_speed_2,
    max_fan_speed_3,
    max_fan_speed_4,
    gpu_card_power,
    gpu_telemetry_count,
};

using GpuTelemetryBitset = std::bitset<static_cast<size_t>(GpuTelemetryCapBits::gpu_telemetry_count)>;