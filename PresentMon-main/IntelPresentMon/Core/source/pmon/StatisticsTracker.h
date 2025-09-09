// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>

namespace p2c::pmon
{
	class StatisticsTracker
	{
	public:
		void Push(double value);
		double GetPercentile(double percentile);
		double GetMin();
		double GetMax();
		double GetMean() const;
		size_t GetCount() const;
		double GetSum() const;
	private:
		void Sort_();
		bool sorted = false;
		std::vector<double> values;
	};
}
