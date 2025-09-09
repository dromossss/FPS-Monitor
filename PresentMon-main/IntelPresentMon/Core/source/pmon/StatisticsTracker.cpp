// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "StatisticsTracker.h"
#include <ranges>
#include <algorithm>
#include <numeric>

namespace rn = std::ranges;
namespace vi = rn::views;

namespace p2c::pmon
{
	void StatisticsTracker::Push(double value)
	{
		values.push_back(value);
		sorted = false;
	}
	double StatisticsTracker::GetPercentile(double percentile)
	{
		Sort_();
		if (values.empty()) {
			return -1.;
		}
		if (values.size() == 1) {
			return values.front();
		}
		if (percentile <= 0.) {
			return values.front();
		}
		if (percentile >= 1.) {
			return values.back();
		}

		double index = (values.size() - 1) * percentile;
		const auto lower = static_cast<size_t>(index);
		const auto upper = lower + 1;
		const auto weight = index - double(lower);

		const double percentileMs = values[lower] * (1 - weight) + values[upper] * weight;
		return percentileMs / 1000.;
	}
	double StatisticsTracker::GetMin()
	{
		Sort_();
		if (values.empty()) {
			return -1.;
		}
		return values.front() / 1000.;
	}
	double StatisticsTracker::GetMax()
	{
		Sort_();
		if (values.empty()) {
			return -1.;
		}
		return values.back() / 1000.;
	}
	double StatisticsTracker::GetMean() const
	{
		if (values.empty()) {
			return -1.;
		}
		const double meanMs = std::accumulate(values.begin(), values.end(), 0.) / GetCount();
		return meanMs / 1000.;
	}
	double StatisticsTracker::GetSum() const
	{
		if (values.empty()) {
			return -1.;
		}
		const double sumMs = std::accumulate(values.begin(), values.end(), 0.);
		return sumMs;
	}
	size_t StatisticsTracker::GetCount() const
	{
		return values.size();
	}
	void StatisticsTracker::Sort_()
	{
		if (!sorted) {
			rn::sort(values);
			sorted = true;
		}
	}
}