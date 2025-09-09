// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Metric.h"

namespace p2c::pmon::met
{
	class SquareWaveMetric : public Metric
	{
	public:
		SquareWaveMetric(std::wstring name, double period, float min, float max);
		void PopulateData(gfx::lay::GraphData& data, double timestamp) override;
		std::optional<float> ReadValue(double timestamp) override;
		const std::wstring& GetCategory() const override;
		const std::wstring& GetMetricClassName() const override;
	private:
		double period;
		float min;
		float max;
	};
}