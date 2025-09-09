// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "SquareWaveMetric.h"
#include <Core/source/gfx/layout/GraphData.h>

namespace p2c::pmon::met
{
	SquareWaveMetric::SquareWaveMetric(std::wstring name, double period, float min, float max)
		:
		Metric{ std::move(name), L"fak" },
		period{ period },
		min{ min },
		max{ max }
	{}

	void SquareWaveMetric::PopulateData(gfx::lay::GraphData& data, double timestamp)
	{
		data.Push(gfx::lay::DataPoint{ .value = ReadValue(timestamp), .time = timestamp });
		data.Trim(timestamp);
	}

	const std::wstring& SquareWaveMetric::GetMetricClassName() const
	{
		static std::wstring name = L"Numeric";
		return name;
	}

	std::optional<float> SquareWaveMetric::ReadValue(double timestamp)
	{
		const auto halfPeriod = period / 2.0;
		const auto pointInCycle = std::fmod(timestamp, period);
		// If the pointInCycle is less than half the period, return max, else return min
		return pointInCycle < halfPeriod ? max : min;
	}

	const std::wstring& SquareWaveMetric::GetCategory() const
	{
		static std::wstring cat = L"Fake Square";
		return cat;
	}
}
