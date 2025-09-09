#pragma once
#include <cstdint>

// TODO: review these definitions; they are legacy code copied from old PresentMonAPI header.
// Replace with more suitable structures that align better with new API or work better without
// the extern "C" restrictions, as necessary/appropriate/convenient
#define MAX_PM_ADAPTER_NAME 64
#define MAX_PM_CPU_NAME 256
#define MAX_PM_PATH 260
#define MAX_RUNTIME_LENGTH 7
#define MAX_PM_FAN_COUNT 5
#define MAX_PM_PSU_COUNT 5
enum PM_GPU_VENDOR {
	PM_GPU_VENDOR_INTEL,
	PM_GPU_VENDOR_NVIDIA,
	PM_GPU_VENDOR_AMD,
	PM_GPU_VENDOR_UNKNOWN
};
enum PM_PSU_TYPE {
    PM_PSU_TYPE_NONE,
    PM_PSU_TYPE_PCIE,
    PM_PSU_TYPE_6PIN,
    PM_PSU_TYPE_8PIN
};
struct PM_ADAPTER_INFO
{
	uint32_t id;
	PM_GPU_VENDOR vendor;
	char name[MAX_PM_ADAPTER_NAME];
	double gpuSustainedPowerLimit;
	uint64_t gpuMemorySize;
	uint64_t gpuMemoryMaxBandwidth;
};
struct PM_FRAME_DATA_OPT_DOUBLE {
    double data;
    bool valid;
};
struct PM_FRAME_DATA_OPT_UINT64 {
    uint64_t data;
    bool valid;
};
struct PM_FRAME_DATA_OPT_INT {
    int data;
    bool valid;
};
struct PM_FRAME_DATA_OPT_PSU_TYPE {
    PM_PSU_TYPE data;
    bool valid;
};
struct PM_FRAME_DATA
{
    // @brief The name of the process that called Present().
    char application[MAX_PM_PATH];

    // @brief The process ID of the process that called Present().
    uint32_t process_id;
    // @brief The address of the swap chain that was presented into.
    uint64_t swap_chain_address;
    // @brief The runtime used to present (e.g., D3D9 or DXGI).
    char runtime[MAX_RUNTIME_LENGTH];
    // @brief The sync interval provided by the application in the Present()
    // call. This value may be modified later by the driver, e.g., based on
    // control panel overrides.
    int32_t sync_interval;
    // @brief Flags used in the Present() call.
    uint32_t present_flags;
    // @brief Whether the frame was dropped (1) or displayed (0).  Note, if
    // dropped, msUntilDisplayed will be 0.
    uint32_t dropped;
    // @brief The time of the Present() call, in seconds, relative to when the
    // PresentMon started recording.
    double time_in_seconds;
    // @brief The time spent inside the Present() call, in milliseconds.
    double ms_in_present_api;
    // @brief The time between this Present() call and the previous one, in
    // milliseconds.
    double ms_between_presents;
    // @brief Whether tearing is possible (1) or not (0).
    uint32_t allows_tearing;
    // @brief The presentation mode used by the system for this Present().
    PM_PRESENT_MODE present_mode;
    // @brief The time between the Present() call and when the GPU work
    // completed, in milliseconds.
    double ms_until_render_complete;
    // @brief The time between the Present() call and when the frame was
    // displayed, in milliseconds.
    double ms_until_displayed;
    // @brief How long the previous frame was displayed before this Present()
    // was displayed, in milliseconds.
    double ms_between_display_change;
    // @brief The time between the Present() call and when the GPU work
    // started, in milliseconds.
    double ms_until_render_start;
    // @brief The time of the Present() call, as a performance counter value.
    uint64_t qpc_time;

    // @brief The time between the Present() call and the earliest keyboard or
    // mouse interaction that contributed to this frame.
    double ms_since_input;
    // @brief The time that any GPU engine was active working on this frame,
    // in milliseconds. Not supported on Win7
    double ms_gpu_active;
    // @brief The time video encode/decode was active separate from the other
    // engines in milliseconds. Not supported on Win7
    double ms_gpu_video_active;

    // Power telemetry
    PM_FRAME_DATA_OPT_DOUBLE gpu_power_w;
    PM_FRAME_DATA_OPT_DOUBLE gpu_sustained_power_limit_w;
    PM_FRAME_DATA_OPT_DOUBLE gpu_voltage_v;
    PM_FRAME_DATA_OPT_DOUBLE gpu_frequency_mhz;
    PM_FRAME_DATA_OPT_DOUBLE gpu_temperature_c;
    PM_FRAME_DATA_OPT_DOUBLE gpu_utilization;
    PM_FRAME_DATA_OPT_DOUBLE gpu_render_compute_utilization;
    PM_FRAME_DATA_OPT_DOUBLE gpu_media_utilization;

    PM_FRAME_DATA_OPT_DOUBLE vram_power_w;
    PM_FRAME_DATA_OPT_DOUBLE vram_voltage_v;
    PM_FRAME_DATA_OPT_DOUBLE vram_frequency_mhz;
    PM_FRAME_DATA_OPT_DOUBLE vram_effective_frequency_gbs;
    PM_FRAME_DATA_OPT_DOUBLE vram_temperature_c;

    PM_FRAME_DATA_OPT_DOUBLE fan_speed_rpm[MAX_PM_FAN_COUNT];

    PM_FRAME_DATA_OPT_PSU_TYPE psu_type[MAX_PM_PSU_COUNT];
    PM_FRAME_DATA_OPT_DOUBLE psu_power[MAX_PM_PSU_COUNT];
    PM_FRAME_DATA_OPT_DOUBLE psu_voltage[MAX_PM_PSU_COUNT];

    // Gpu memory telemetry
    PM_FRAME_DATA_OPT_UINT64 gpu_mem_total_size_b;
    PM_FRAME_DATA_OPT_UINT64 gpu_mem_used_b;
    PM_FRAME_DATA_OPT_UINT64 gpu_mem_max_bandwidth_bps;
    PM_FRAME_DATA_OPT_DOUBLE gpu_mem_read_bandwidth_bps;
    PM_FRAME_DATA_OPT_DOUBLE gpu_mem_write_bandwidth_bps;

    // Throttling flags
    PM_FRAME_DATA_OPT_INT gpu_power_limited;
    PM_FRAME_DATA_OPT_INT gpu_temperature_limited;
    PM_FRAME_DATA_OPT_INT gpu_current_limited;
    PM_FRAME_DATA_OPT_INT gpu_voltage_limited;
    PM_FRAME_DATA_OPT_INT gpu_utilization_limited;

    PM_FRAME_DATA_OPT_INT vram_power_limited;
    PM_FRAME_DATA_OPT_INT vram_temperature_limited;
    PM_FRAME_DATA_OPT_INT vram_current_limited;
    PM_FRAME_DATA_OPT_INT vram_voltage_limited;
    PM_FRAME_DATA_OPT_INT vram_utilization_limited;

    // Cpu Telemetry
    PM_FRAME_DATA_OPT_DOUBLE cpu_utilization;
    PM_FRAME_DATA_OPT_DOUBLE cpu_power_w;
    PM_FRAME_DATA_OPT_DOUBLE cpu_power_limit_w;
    PM_FRAME_DATA_OPT_DOUBLE cpu_temperature_c;
    PM_FRAME_DATA_OPT_DOUBLE cpu_frequency;
};