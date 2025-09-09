// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "SignatureComparison.h"
#include "NvapiWrapper.h"
#include "NvmlWrapper.h"

namespace pwr::nv
{
	bool operator==(const NvapiAdapterSignature& nvapi, const NvmlAdapterSignature& nvml) noexcept
	{
		// pci identifier test has priority
		if (nvapi.deviceId == nvml.pciDeviceId && nvapi.subSystemId == nvml.pciSubSystemId)
		{
			return true;
		}
		// if pci identifier test fails, fallback to nvml domain/bus/function test
		if (nvapi.busId == nvml.busIdBus)
		{
			return true;
		}
		return false;
	}
}