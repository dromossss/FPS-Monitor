// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <cstdint>

namespace pmon::util
{
	int64_t GetCurrentTimestamp() noexcept;
	double GetTimestampPeriodSeconds() noexcept;
	void SpinWaitUntilTimestamp(int64_t timestamp) noexcept;
	double TimestampDeltaToSeconds(int64_t start, int64_t end, double period) noexcept;

	class QpcTimer
	{
	public:
		QpcTimer() noexcept;
		double Mark() noexcept;
		double Peek() const noexcept;
		int64_t GetStartTimestamp() const noexcept;
		void SpinWaitUntil(double seconds) const noexcept;
	private:
		double performanceCounterPeriod_;
		int64_t startTimestamp_ = 0;
	};
}