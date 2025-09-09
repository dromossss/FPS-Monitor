// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Selector.h"
#include <ranges>
#include <algorithm>

namespace p2c::gfx::lay::sty
{
	Selector::Selector(std::vector<std::string> parentClasses_, std::vector<std::string> targetClasses_)
		:
		parentClasses{ std::move(parentClasses_) },
		targetClasses{ std::move(targetClasses_) }
	{
		std::ranges::sort(parentClasses);
		std::ranges::sort(targetClasses);
	}
	bool Selector::ParentMatches(const std::vector<std::string>& classes) const
	{
		return std::ranges::includes(classes, parentClasses);
	}
	bool Selector::TargetMatches(const std::vector<std::string>& classes) const
	{
		return std::ranges::includes(classes, targetClasses);
	}
	bool sty::Selector::operator>(const Selector& rhs) const
	{
		if (targetClasses.size() == rhs.targetClasses.size())
		{
			return parentClasses.size() > rhs.parentClasses.size();
		}
		return targetClasses.size() > rhs.targetClasses.size();
	}
}