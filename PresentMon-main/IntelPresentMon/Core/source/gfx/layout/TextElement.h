// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Element.h"
#include "Enums.h"

namespace p2c::gfx::prim
{
	class TextStylePrimitive;
	class TextPrimitive;
}

namespace p2c::gfx::lay
{
	class TextElement : public Element
	{
	public:
		TextElement(std::wstring text, std::vector<std::string> classes = {});
		~TextElement();
		void SetText(std::wstring newText);
		static std::shared_ptr<TextElement> Make(std::wstring text, std::vector<std::string> classes = {});
	protected:
		LayoutConstraints QueryLayoutConstraints_(std::optional<float> width, sty::StyleProcessor& sp, Graphics& gfx) const override;
		void SetDimension_(float dimension, FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx) override;
		void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) override;
		void Draw_(Graphics& gfx) const override;
	private:
		// functions
		void UpdateLayoutCache(const Dimensions& available, sty::StyleProcessor& sp, Graphics& gfx) const;
		// data
		std::wstring text;
		mutable bool textDirty = true;
		mutable std::unique_ptr<prim::TextPrimitive> pText;
	};
}