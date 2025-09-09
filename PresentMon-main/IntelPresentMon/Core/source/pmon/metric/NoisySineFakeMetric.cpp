// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "NoisySineFakeMetric.h"
#include <Core/source/gfx/layout/GraphData.h>

namespace p2c::pmon::met
{
	NoisySineFakeMetric::NoisySineFakeMetric(std::wstring name, float freq, float phase, float ampli, float offset, float dev, float errScale)
		:
		Metric{ std::move(name), L"fak" },
		freq{ freq },
		phase{ phase },
		ampli{ ampli },
		offset{ offset },
		errScale{ errScale },
		rng{ std::random_device{}() },
		dist{ 0.f, dev }
	{}

	void NoisySineFakeMetric::PopulateData(gfx::lay::GraphData& data, double timestamp)
	{
		data.Push(gfx::lay::DataPoint{ .value = ReadValue(timestamp), .time = timestamp});
		data.Trim(timestamp);
	}

	const std::wstring& NoisySineFakeMetric::GetMetricClassName() const
	{
		static std::wstring name = L"Numeric";
		return name;
	}

	std::optional<float> NoisySineFakeMetric::ReadValue(double timestamp)
	{
		return ampli * std::sin(float(timestamp) * 2.f * 3.14159f * freq + phase) + offset + errScale * dist(rng);
	}

	const std::wstring& NoisySineFakeMetric::GetCategory() const
	{
		static std::wstring cat = L"Fake Sine";
		return cat;
	}
}
