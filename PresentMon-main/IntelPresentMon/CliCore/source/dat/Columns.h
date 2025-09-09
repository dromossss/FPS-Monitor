#pragma once

#define COLUMN_LIST \
X_("Application", "", application,,, core) \
X_("ProcessID", "", process_id,,, core) \
X_("SwapChainAddress", "", swap_chain_address,,, core) \
X_("Runtime", "", runtime,,, core) \
X_("SyncInterval", "", sync_interval,,, core) \
X_("PresentFlags", "", present_flags,,, core) \
X_("Dropped", "", dropped,,, core) \
X_("TimeInSeconds", "[s]", time_in_seconds,,, core) \
X_("QPCTime", "", qpc_time,,, core) \
X_("msInPresentAPI", "[ms]", ms_in_present_api,,, core) \
X_("msBetweenPresents", "[ms]", ms_between_presents,,, core) \
X_("AllowsTearing", "", allows_tearing,,, core) \
X_("PresentMode", "", present_mode, TransformPresentMode,, core) \
X_("msUntilRenderComplete", "[ms]", ms_until_render_complete,,, core) \
X_("msUntilDisplayed", "[ms]", ms_until_displayed,,, core) \
X_("msBetweenDisplayChange", "[ms]", ms_between_display_change,,, core) \
X_("msUntilRenderStart", "[ms]", ms_until_render_start,,, track_gpu) \
X_("msGPUActive", "[ms]", ms_gpu_active,,, track_gpu) \
X_("msGPUVideoActive", "[ms]", ms_gpu_video_active,,, track_gpu_video) \
X_("msSinceInput", "[ms]", ms_since_input,,, track_input) \
X_("GPUPower", "[W]", gpu_power_w,,, track_gpu_telemetry) \
X_("GPUSustainedPowerLimit", "[W]", gpu_sustained_power_limit_w,,, track_gpu_telemetry) \
X_("GPUVoltage", "[V]", gpu_voltage_v,,, track_gpu_telemetry) \
X_("GPUFrequency", "[MHz]", gpu_frequency_mhz,,, track_gpu_telemetry) \
X_("GPUTemperature", "[C]", gpu_temperature_c,,, track_gpu_telemetry) \
X_("GPUUtilization", "[%]", gpu_utilization,,, track_gpu_telemetry) \
X_("GPURenderComputeUtilization", "[%]", gpu_render_compute_utilization,,, track_gpu_telemetry) \
X_("GPUMediaUtilization", "[%]", gpu_media_utilization,,, track_gpu_telemetry) \
X_("VRAMPower", "[W]", vram_power_w,,, track_vram_telemetry) \
X_("VRAMVoltage", "[V]", vram_voltage_v,,, track_vram_telemetry) \
X_("VRAMFrequency", "[MHz]", vram_frequency_mhz,,, track_vram_telemetry) \
X_("VRAMEffectiveFrequency", "[Gbps]", vram_effective_frequency_gbs,,, track_vram_telemetry) \
X_("VRAMTemperature", "[C]", vram_temperature_c,,, track_vram_telemetry) \
X_("GPUMemTotalSize", "[B]", gpu_mem_total_size_b,,, track_gpu_memory) \
X_("GPUMemUsed", "[B]", gpu_mem_used_b,,, track_gpu_memory) \
X_("GPUMemMaxBandwidth", "[bps]", gpu_mem_max_bandwidth_bps,,, track_gpu_memory) \
X_("GPUMemReadBandwidth", "[bps]", gpu_mem_read_bandwidth_bps,,, track_gpu_memory) \
X_("GPUMemWriteBandwidth", "[bps]", gpu_mem_write_bandwidth_bps,,, track_gpu_memory) \
X_("GPUFanSpeed0", "[rpm]", fan_speed_rpm,, [0], track_gpu_fan) \
X_("GPUFanSpeed1", "[rpm]", fan_speed_rpm,, [1], track_gpu_fan) \
X_("GPUFanSpeed2", "[rpm]", fan_speed_rpm,, [2], track_gpu_fan) \
X_("GPUFanSpeed3", "[rpm]", fan_speed_rpm,, [3], track_gpu_fan) \
X_("GPUFanSpeed4", "[rpm]", fan_speed_rpm,, [4], track_gpu_fan) \
X_("PSUType0", "", psu_type,, [0], track_gpu_psu) \
X_("PSUType1", "", psu_type,, [1], track_gpu_psu) \
X_("PSUType2", "", psu_type,, [2], track_gpu_psu) \
X_("PSUType3", "", psu_type,, [3], track_gpu_psu) \
X_("PSUType4", "", psu_type,, [4], track_gpu_psu) \
X_("PSUPower0", "[W]", psu_power,, [0], track_gpu_psu) \
X_("PSUPower1", "[W]", psu_power,, [1], track_gpu_psu) \
X_("PSUPower2", "[W]", psu_power,, [2], track_gpu_psu) \
X_("PSUPower3", "[W]", psu_power,, [3], track_gpu_psu) \
X_("PSUPower4", "[W]", psu_power,, [4], track_gpu_psu) \
X_("PSUVoltage0", "[V]", psu_voltage,, [0], track_gpu_psu) \
X_("PSUVoltage1", "[V]", psu_voltage,, [1], track_gpu_psu) \
X_("PSUVoltage2", "[V]", psu_voltage,, [2], track_gpu_psu) \
X_("PSUVoltage3", "[V]", psu_voltage,, [3], track_gpu_psu) \
X_("PSUVoltage4", "[V]", psu_voltage,, [4], track_gpu_psu) \
X_("GPUPowerLimited", "", gpu_power_limited,,, track_perf_limit) \
X_("GPUTemperatureLimited", "", gpu_temperature_limited,,, track_perf_limit) \
X_("GPUCurrentLimited", "", gpu_current_limited,,, track_perf_limit) \
X_("GPUVoltageLimited", "", gpu_voltage_limited,,, track_perf_limit) \
X_("GPUUtilizationLimited", "", gpu_utilization_limited,,, track_perf_limit) \
X_("VRAMPowerLimited", "", vram_power_limited,,, track_perf_limit) \
X_("VRAMTemperatureLimited", "", vram_temperature_limited,,, track_perf_limit) \
X_("VRAMCurrentLimited", "", vram_current_limited,,, track_perf_limit) \
X_("VRAMVoltageLimited", "", vram_voltage_limited,,, track_perf_limit) \
X_("VRAMUtilizationLimited", "", vram_utilization_limited,,, track_perf_limit) \
X_("CPUUtilization", "[%]", cpu_utilization,,, track_cpu_telemetry) \
X_("CPUPower", "[W]", cpu_power_w,,, track_cpu_telemetry) \
X_("CPUPowerLimit", "[W]", cpu_power_limit_w,,, track_cpu_telemetry) \
X_("CPUTemperature", "[C]", cpu_temperature_c,,, track_cpu_telemetry) \
X_("CPUFrequency", "[GHz]", cpu_frequency,,, track_cpu_telemetry)