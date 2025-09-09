// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>

namespace pwr::nv
{
	struct NvapiAdapterSignature;
	struct NvmlAdapterSignature;

	bool operator==(const NvapiAdapterSignature& nvapi, const NvmlAdapterSignature& nvml) noexcept;
}