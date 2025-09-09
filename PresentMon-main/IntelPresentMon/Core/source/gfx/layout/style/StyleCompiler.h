// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Stylesheet.h"


namespace p2c::gfx::lay::sty
{
	// TODO: keep track of sheets in terms of distance to activating parent
	// use distance to sort for merging priority
	class StyleCompiler
	{
		// sheet with flag to show whether it satisfies parent requirement
		struct FlaggedSheet
		{
			FlaggedSheet(std::shared_ptr<Stylesheet> pSheet_);
			bool IsActive() const;
			void Deactivate();
			bool operator>(const FlaggedSheet& rhs) const;

			// data members
			std::shared_ptr<Stylesheet> pSheet;
			// index of the earliest element in DOM parent stack that activates this sheet
			// -2 signals inactive
			// -1 signals active by default (no parent selector)
			int activatedIndex = -2;
		};
	public:
		StyleCompiler() = default;
		StyleCompiler(std::vector<std::shared_ptr<Stylesheet>> sheets_);
		void PushParent(std::vector<std::string> classes);
		void PopParent();
		void SetSheets(std::vector<std::shared_ptr<Stylesheet>> sheets_);
		std::shared_ptr<Stylesheet> Compile(const std::vector<std::string>& elementClasses) const;
	private:
		std::vector<FlaggedSheet> sheets;
		std::vector<std::vector<std::string>> parentStack;
	};
}