// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "ReadoutElement.h"
#include "TextElement.h"
#include "GraphData.h"
#include <limits>


namespace p2c::gfx::lay
{
	ReadoutElement::ReadoutElement(bool textOutput_, std::wstring label_, std::wstring units_, std::wstring* pText_, std::vector<std::string> classes)
		:
		FlexElement{ {}, [&classes]{ classes.push_back("$readout"); return std::move(classes); }() },
		label{ std::move(label_) },
		pValueText{ pText_ },
		units{ std::move(units_) },
		textOutput{ textOutput_ }
	{
		AddChild(TextElement::Make(label, { "$text-large", "$label"}));
		if (textOutput)
		{
			AddChild(pVal = TextElement::Make(L"", {"$text-large", "$text-value"}));
		}
		else
		{
			AddChild(pVal = TextElement::Make(L"0000", { "$text-large", "$numeric-value" }));
			AddChild(TextElement::Make(units, { "$numeric-units" }));
		}
	}

	void ReadoutElement::Draw_(Graphics& gfx) const
	{
		if (*pValueText != lastValueText)
		{
			pVal->SetText(*pValueText);
			lastValueText = *pValueText;
		}
		FlexElement::Draw_(gfx);
	}

	ReadoutElement::~ReadoutElement() {}

	std::shared_ptr<Element> ReadoutElement::Make(bool textOutput, std::wstring label, std::wstring units, std::wstring* pText, std::vector<std::string> classes)
	{
		return std::make_shared<ReadoutElement>(textOutput, std::move(label), std::move(units), pText, std::move(classes));
	}
}