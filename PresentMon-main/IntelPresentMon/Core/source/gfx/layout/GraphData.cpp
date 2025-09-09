// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "GraphData.h"
#include <algorithm>
#include <ranges>

namespace rn = std::ranges;

namespace p2c::gfx::lay
{
	GraphData::GraphData(double timeWindow)
		:
		timeWindow{ timeWindow }
	{}
	DataPoint& GraphData::operator[](size_t i)
	{
		return data[i];
	}
	const DataPoint& GraphData::operator[](size_t i) const
	{
		return data[i];
	}
	DataPoint& GraphData::Front()
	{
		return data.front();
	}
	const DataPoint& GraphData::Front() const
	{
		return data.front();
	}
	DataPoint& GraphData::Back()
	{
		return data.back();
	}
	const DataPoint& GraphData::Back() const
	{
		return data.back();
	}
	void GraphData::Push(const DataPoint& dp)
	{
		data.push_front(dp);
		if (dp.value.has_value()) {
			min.Push(*dp.value);
			max.Push(*dp.value);
		}
	}
	size_t GraphData::Size() const
	{
		return data.size();
	}
	void GraphData::Trim(double now)
	{
		const auto cutoff = now - timeWindow;
		// find iterator pair that marks all data points that are
		// outside the time window, except the last one, by reverse scan
		auto ri = data.rbegin();
		const auto rend = data.rend();
		while (rend - ri > 2 && std::next(ri)->time < cutoff) {
			ri++;
		}
		// convert iterator pair to normal iterators
		const auto i = ri.base();
		const auto end = data.end();
		// remove all in range from the extreme queue
		for (const auto& d : rn::subrange{ data.rbegin(), ri }) {
			if (d.value.has_value()) {
				min.Pop(*d.value);
				max.Pop(*d.value);
			}
		}
		// erase all in range from data buffer
		data.erase(i, end);
	}
	void GraphData::Resize(double window)
	{
		timeWindow = window;
	}
	std::optional<float> GraphData::Min() const
	{
		return min.GetCurrent();
	}
	std::optional<float> GraphData::Max() const
	{
		return max.GetCurrent();
	}
	double GraphData::GetWindowSize() const
	{
		return timeWindow;
	}
}