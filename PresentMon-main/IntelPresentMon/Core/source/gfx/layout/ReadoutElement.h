// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "FlexElement.h"


namespace p2c::pmon
{
	class Metric;
}

namespace p2c::gfx::lay
{
	class TextElement;

	class ReadoutElement : public FlexElement
	{
	public:
		// functions
		ReadoutElement(bool textOutput, std::wstring label, std::wstring units, std::wstring* pText, std::vector<std::string> classes = {});
        ReadoutElement(const ReadoutElement&) = delete;
        ReadoutElement& operator=(const ReadoutElement&) = delete;
		~ReadoutElement() override;
		static std::shared_ptr<Element> Make(bool textOutput, std::wstring label, std::wstring units, std::wstring* pText, std::vector<std::string> classes = {});
	protected:
		void Draw_(Graphics& gfx) const override;
	private:
		std::shared_ptr<TextElement> pVal;
		std::wstring* pValueText;
		std::wstring label;
		std::wstring units;
		bool textOutput;
		mutable std::wstring lastValueText;
	};
}