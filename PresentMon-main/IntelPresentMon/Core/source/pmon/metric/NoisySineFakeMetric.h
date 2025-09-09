// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <random>
#include "Metric.h"

namespace p2c::pmon::met
{
	class NoisySineFakeMetric : public Metric
	{
	public:
		NoisySineFakeMetric(std::wstring name, float freq, float phase, float ampli, float offset, float dev, float errScale);
		void PopulateData(gfx::lay::GraphData& data, double timestamp) override;
		std::optional<float> ReadValue(double timestamp) override;
		const std::wstring& GetCategory() const override;
		const std::wstring& GetMetricClassName() const override;
	private:
		float freq;
		float phase;
		float ampli;
		float offset;
		float errScale;
		std::minstd_rand rng;
		std::normal_distribution<float> dist;
	};
}