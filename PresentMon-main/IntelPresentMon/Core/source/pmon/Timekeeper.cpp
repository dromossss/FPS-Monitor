// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Timekeeper.h"
#include <CommonUtilities/Qpc.h>

namespace p2c::pmon
{
	using namespace ::pmon::util;

	double Timekeeper::Now()
	{
		return RelativeToEpoch(GetCurrentTimestamp());
	}

	void Timekeeper::LockNow()
	{
		singleton.locked = Now();
	}

	double Timekeeper::GetLockedNow()
	{
		return singleton.locked;
	}

	double Timekeeper::RelativeToEpoch(int64_t qpc)
	{
		return singleton.period * double(qpc - singleton.epoch);
	}

	Timekeeper::Timekeeper()
		:
		epoch{ GetCurrentTimestamp() },
		period{ GetTimestampPeriodSeconds() }
	{}

	Timekeeper Timekeeper::singleton{};
}
