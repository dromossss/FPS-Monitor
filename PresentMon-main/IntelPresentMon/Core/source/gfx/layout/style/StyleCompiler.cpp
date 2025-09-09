// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "StyleCompiler.h"
#include <algorithm>


namespace p2c::gfx::lay::sty
{
	StyleCompiler::FlaggedSheet::FlaggedSheet(std::shared_ptr<Stylesheet> pSheet_)
		:
		pSheet{ std::move(pSheet_) }
	{
		// setting all sheets without parent selector as unconditionally active
		if (pSheet->GetSelector().ParentMatches({}))
		{
			activatedIndex = -1;
		}
	}
	bool StyleCompiler::FlaggedSheet::IsActive() const
	{
		return activatedIndex > -2;
	}
	void StyleCompiler::FlaggedSheet::Deactivate()
	{
		activatedIndex = -2;
	}
	bool StyleCompiler::FlaggedSheet::operator>(const FlaggedSheet& rhs) const
	{
		return *pSheet > *rhs.pSheet;
	}



	StyleCompiler::StyleCompiler(std::vector<std::shared_ptr<Stylesheet>> sheets_)
	{
		SetSheets(std::move(sheets_));
	}

	void StyleCompiler::PushParent(std::vector<std::string> classes)
	{
		for (auto& s : sheets)
		{
			if (!s.IsActive() && s.pSheet->GetSelector().ParentMatches(classes))
			{
				s.activatedIndex = (int)parentStack.size();
			}
		}
		parentStack.push_back(std::move(classes));
	}
	void StyleCompiler::PopParent()
	{
		parentStack.pop_back();
		for (auto& s : sheets)
		{
			if (s.activatedIndex == (int)parentStack.size())
			{
				s.Deactivate();
			}
		}
	}
	void StyleCompiler::SetSheets(std::vector<std::shared_ptr<Stylesheet>> sheets_)
	{
		sheets.clear();
		for (auto& s : sheets_)
		{
			sheets.push_back({ std::move(s) });
			for (size_t i = 0; i < parentStack.size(); i++)
			{
				if (sheets.back().pSheet->GetSelector().ParentMatches(parentStack[i]))
				{
					sheets.back().activatedIndex = (int)i;
				}
			}
		}
		std::stable_sort(sheets.begin(), sheets.end(), std::greater{});
	}
	std::shared_ptr<Stylesheet> StyleCompiler::Compile(const std::vector<std::string>& elementClasses) const
	{
		auto pCompiled = Stylesheet::Make();
		for (auto& s : sheets)
		{
			if (s.IsActive() && s.pSheet->GetSelector().TargetMatches(elementClasses))
			{
				pCompiled->MergeFrom(*s.pSheet);
			}
		}
		return pCompiled;
	}
}