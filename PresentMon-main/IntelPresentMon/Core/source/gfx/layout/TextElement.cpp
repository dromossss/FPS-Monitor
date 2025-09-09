// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "TextElement.h"
#include "AxisMapping.h"
#include <Core/source/gfx/prim/TextPrimitive.h>
#include <Core/source/gfx/prim/TextStylePrimitive.h>
#include "style/StyleProcessor.h"

namespace p2c::gfx::lay
{
	TextElement::TextElement(std::wstring text, std::vector<std::string> classes_)
		:
		Element{ std::move(classes_) },
		text{ std::move(text) }
	{}

	TextElement::~TextElement()
	{}

	LayoutConstraints TextElement::QueryLayoutConstraints_(std::optional<float> width, sty::StyleProcessor& sp, Graphics& gfx) const
	{
		const auto flexGrow = sp.Resolve<sty::at::flexGrow>();

		const Dimensions available{ width.value_or(unlimited), unlimited };

		UpdateLayoutCache(available, sp, gfx);
		const auto metrics = pText->GetActualDimensions();

		if (width)
		{
			return { .basis = metrics.height, .flexGrow = flexGrow };
		}
		else
		{
			return { .basis = metrics.width, .flexGrow = flexGrow };
		}
	}

	void TextElement::SetDimension_(float dimension, FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx)
	{
		DimensionsSpec newDimensions;
		if (pText)
		{
			// read in existing dimensions that have been set, if there is a layout
			newDimensions = pText->GetMaxDimensions();
		}
		// overwrite existing (if any) with dimension currently being set
		map::DimSpecScalar(newDimensions, dir) = dimension;
		UpdateLayoutCache({
			newDimensions.width.value_or(unlimited),
			newDimensions.height.value_or(unlimited)
		}, sp, gfx);
	}

	void TextElement::SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx)
	{
		pText->SetPosition(pos);
	}

	void TextElement::Draw_(Graphics& gfx) const
	{
		if (textDirty)
		{
			pText->SetText(text, gfx);
			textDirty = false;
		}

		pText->Draw(gfx);
	}

	void TextElement::SetText(std::wstring newText)
	{
		text = std::move(newText);
		textDirty = true;
	}

	std::shared_ptr<TextElement> TextElement::Make(std::wstring text, std::vector<std::string> classes)
	{
		return std::make_shared<TextElement>(std::move(text), std::move(classes));
	}

	void TextElement::UpdateLayoutCache(const Dimensions& available, sty::StyleProcessor& sp, Graphics& gfx) const
	{
		auto& cl = GetClasses();
		if (std::ranges::find(cl, "$footer-center-left") != cl.end())
		{
			int x = 0;
		}

		if (pText)
		{
			pText->SetMaxDimensions(available);
		}
		else
		{
			using namespace sty;
			const auto textFont = sp.Resolve<at::textFont>();
			const auto textSize = sp.Resolve<at::textSize>();
			const auto textStyle = sp.Resolve<at::textStyle>();
			const auto textWeight = sp.Resolve<at::textWeight>();
			const auto textAlignment = sp.Resolve<at::textAlignment>();
			const auto textJustification = sp.Resolve<at::textJustification>();
			const auto textColor = sp.Resolve<at::textColor>();

			// TODO: eliminate TextStylePrimative, fold functionality into TextElement (maybe?)
			auto pStyle = std::make_unique<prim::TextStylePrimitive>(textFont, textSize, gfx, textStyle, textWeight);
			pStyle->SetAlignment(textAlignment);
			pStyle->SetJustification(textJustification);
			pText = std::make_unique<prim::TextPrimitive>(
				text, *pStyle, available, 
				std::make_shared<prim::BrushPrimitive>(textColor, gfx), gfx
			);
			textDirty = false;
		}
	}
}