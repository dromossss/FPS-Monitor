#pragma once
#include "../Interprocess/source/Interprocess.h"

namespace pmon::ipc::intro
{
	inline void RegisterMockIntrospectionDevices(ServiceComms& comms)
	{
		using namespace std::string_literals;
		{
			CpuTelemetryBitset caps;
			caps.set(size_t(CpuTelemetryCapBits::cpu_utilization));
			comms.RegisterCpuDevice(PM_DEVICE_VENDOR_INTEL, "Core i7 4770k"s, caps);
		}
		{
			GpuTelemetryBitset caps;
			caps.set(size_t(GpuTelemetryCapBits::gpu_power));
			caps.set(size_t(GpuTelemetryCapBits::fan_speed_0));
			comms.RegisterGpuDevice(PM_DEVICE_VENDOR_INTEL, "Arc 750"s, caps);
		}
		{
			GpuTelemetryBitset caps;
			caps.set(size_t(GpuTelemetryCapBits::gpu_power));
			caps.set(size_t(GpuTelemetryCapBits::fan_speed_0));
			caps.set(size_t(GpuTelemetryCapBits::fan_speed_1));
			comms.RegisterGpuDevice(PM_DEVICE_VENDOR_NVIDIA, "GeForce RTX 2080 ti"s, caps);
		}
		comms.FinalizeGpuDevices();
	}
}