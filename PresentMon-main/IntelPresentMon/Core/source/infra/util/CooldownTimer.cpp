// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "CooldownTimer.h"
#include <cassert>


namespace p2c::infra::util
{
	CooldownTimer::CooldownTimer(float period, size_t threshold)
		: period_{ period }, threshold_{ threshold }
	{
		assert(threshold > 0);
	}

	bool CooldownTimer::Trigger()
	{
		if (instances_.empty())
		{
			timer_.Mark();
			instances_.push_back(0.f);
			return false;
		}

		instances_.push_back(float(timer_.Peek()));

		if (ThresholdExceeded())
		{
			// if we're above the threshold, run the update just in case
			// there are stale entries and then check again to be sure
			Update();
			if (ThresholdExceeded())
			{
				return true;
			}
		}
		return false;
	}

	bool CooldownTimer::ThresholdExceeded() const
	{
		return instances_.size() > threshold_;
	}

	void CooldownTimer::Update()
	{
		const auto t = timer_.Peek();
		while (!instances_.empty() && t - instances_.front() > period_)
		{
			instances_.pop_front();
		}
	}
}