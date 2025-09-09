// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>
#include <string>


namespace p2c::gfx::lay::sty
{
	class Selector
	{
	public:
		Selector() = default;
		Selector(std::vector<std::string> parentClasses_, std::vector<std::string> targetClasses_);
		bool ParentMatches(const std::vector<std::string>& classes) const;
		bool TargetMatches(const std::vector<std::string>& classes) const;
		bool operator>(const Selector& rhs) const;
	private:
		std::vector<std::string> parentClasses;
		std::vector<std::string> targetClasses;
	};
}