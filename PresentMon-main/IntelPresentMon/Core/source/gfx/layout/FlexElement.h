// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "Element.h"
#include "Enums.h"

// specialization of Element that renders it's children with flexbox-like layout rules

namespace p2c::gfx::lay
{
	class FlexElement : public Element
	{
	public:
		FlexElement(std::vector<std::shared_ptr<Element>> children, std::vector<std::string> classes = {});
		static std::shared_ptr<Element> Make(std::vector<std::shared_ptr<Element>> children, std::vector<std::string> classes = {})
		{
			return std::make_shared<FlexElement>(std::move(children), std::move(classes));
		}
	protected:
		LayoutConstraints QueryLayoutConstraints_(std::optional<float> width, sty::StyleProcessor& sp, Graphics& gfx) const override;
		void SetDimension_(float dimension, FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx) override;
		void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) override;
		void Draw_(Graphics& gfx) const override;
	};
}