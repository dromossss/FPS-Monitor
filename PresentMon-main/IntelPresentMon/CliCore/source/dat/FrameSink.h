// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once

struct PM_FRAME_DATA;

namespace p2c::cli::dat
{
	class FrameSink
	{
	public:
		virtual void Process(const PM_FRAME_DATA&) = 0;
	};
}