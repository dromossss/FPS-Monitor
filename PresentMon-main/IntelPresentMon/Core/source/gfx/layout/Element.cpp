// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Element.h"
#include "style/StyleProcessor.h"
#include "style/ReadOnlyAttributes.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/Exception.h>
#include "AxisMapping.h"
#include "../prim/BrushPrimitive.h"
#include "../prim/RectPrimitive.h"
#include "../prim/RectBorderPrimitive.h"
#include "../prim/RectMultiBorderPrimitive.h"
#include <algorithm>
#include <ranges>

namespace p2c::gfx::lay
{
	using namespace ::pmon::util;
	using namespace sty;

	Element::Element(std::vector<std::string> classes_)
		:
		classes{ std::move(classes_) }
	{
		std::ranges::sort(classes);
	}

	LayoutConstraints Element::QueryLayoutConstraints(FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx) const
	{
		// query will be called only on the children in the other layout functions, never the current element, so we're okay to push here
		const auto pushTok = sp.Push(this);

		CacheAttributes(sp);

		if (dir == FlexDirection::Column && !boxDimensions.width)
		{
			pmlog_warn("querying element height constraints before width set");
		}

		if (attrCache->display == Display::None)
		{
			return { 0.f, 0.f, 0.f };
		}

		const auto innerSkirtingSize = map::DimsScalar((attrCache->padding + attrCache->border).ToDimensions(), dir);
		const auto marginSize = map::DimsScalar(attrCache->margin.ToDimensions(), dir);

		// if size spec'd, just return spec'd size + margin
		if (auto size = map::DimSpecScalar(sp.Resolve<at::size>(), dir))
		{
			return LayoutConstraints{
				.min = *size + marginSize,
				.basis = *size + marginSize
			};
		}

		// if size auto, calculate based on skirting and content
		const auto queryWidth = dir == FlexDirection::Row ? std::optional<float>{} : std::optional<float>{ *boxDimensions.width };
		// calculate content constraints
		auto constraints = QueryLayoutConstraints_(queryWidth, sp, gfx);
		// add skirting to content constraints
		constraints.min += innerSkirtingSize + marginSize;
		constraints.basis += innerSkirtingSize + marginSize;

		return constraints;
	}

	void Element::SetPosition(const Vec2& pos, sty::StyleProcessor& sp, Graphics& gfx)
	{
		const auto pushTok = sp.Push(this);

		if (!boxDimensions)
		{
			pmlog_error("Attempt to set element position without dimensions");
			throw Except<Exception>();
		}

		if (attrCache->display == Display::None)
		{
			boxPosition = pos;
			return;
		}

		boxPosition = pos + Vec2{ attrCache->margin.left, attrCache->margin.top };

		const auto borderColorLeft = sp.Resolve<at::borderColorLeft>();
		const auto borderColorTop = sp.Resolve<at::borderColorTop>();
		const auto borderColorRight = sp.Resolve<at::borderColorRight>();
		const auto borderColorBottom = sp.Resolve<at::borderColorBottom>();

		// all parameters should be determined now; time to construct the graphics primitives
		if (attrCache->border && (borderColorLeft.IsVisible() || borderColorTop.IsVisible() || borderColorRight.IsVisible() || borderColorBottom.IsVisible()))
		{
			// TODO: make this better by ignoring invisible or zero size borders for the purpose of checking consensus
			if (borderColorLeft == borderColorTop && borderColorLeft == borderColorRight && borderColorLeft == borderColorBottom)
			{
				pBorder = std::make_unique<prim::RectBorderPrimitive>(GetBoxRect(), attrCache->border, std::make_shared<prim::BrushPrimitive>(borderColorLeft, gfx), gfx);
			}
			else
			{
				pBorder = std::make_unique<prim::RectMultiBorderPrimitive>(GetBoxRect(), attrCache->border,
					std::make_shared<prim::BrushPrimitive>(borderColorLeft, gfx),
					std::make_shared<prim::BrushPrimitive>(borderColorTop, gfx),
					std::make_shared<prim::BrushPrimitive>(borderColorRight, gfx),
					std::make_shared<prim::BrushPrimitive>(borderColorBottom, gfx),
				gfx);
			}
		}
		auto backgroundColor = sp.Resolve<sty::at::backgroundColor>();
		if (GetContentRect().GetDimensions() && backgroundColor.IsVisible())
		{
			pBackground = std::make_unique<prim::RectPrimitive>(GetBackgroundRect(), std::make_shared<prim::BrushPrimitive>(backgroundColor, gfx), gfx);
		}

		{
			const auto contentRect = GetContentRect();
			SetPosition_(contentRect.GetTopLeft(), contentRect.GetDimensions(), sp, gfx);
		}
	}

	void Element::SetDimension(float dimension, FlexDirection setdir, sty::StyleProcessor& sp, Graphics& gfx)
	{
		const auto pushTok = sp.Push(this);

		CacheAttributes(sp);

		if (attrCache->display == Display::None)
		{
			map::DimSpecScalar(boxDimensions, setdir) = 0.f;
			return;
		}

		auto const marginSize = map::DimsScalar(attrCache->margin.ToDimensions(), setdir);

		map::DimSpecScalar(boxDimensions, setdir) = dimension - marginSize;

		{
			auto const totalSkirtSize = map::DimsScalar((attrCache->border + attrCache->padding).ToDimensions(), setdir) + marginSize;
			SetDimension_(dimension - totalSkirtSize, setdir, sp, gfx);
		}
	}

	void Element::Draw(Graphics& gfx) const
	{
		if (attrCache->display == Display::Visible)
		{
			if (pBorder)
			{
				pBorder->Draw(gfx);
			}
			if (pBackground)
			{
				pBackground->Draw(gfx);
			}

			Draw_(gfx);
		}
	}

	Element::~Element()
	{}

	Rect Element::GetBoxRect() const
	{
		if (!IsLaidOut())
		{
			pmlog_error("Trying to access element rect before laid out");
			throw Except<Exception>();
		}
		return { *boxPosition, { *boxDimensions.width, *boxDimensions.height } };
	}

	Rect Element::GetBackgroundRect() const
	{
		return GetBoxRect().Augment(-attrCache->border);
	}

	Rect Element::GetElementRect() const
	{
		return GetBoxRect().Augment(attrCache->margin);
	}

	Rect Element::GetContentRect() const
	{
		return GetBoxRect().Augment(-(attrCache->border + attrCache->padding));
	}

	Dimensions Element::GetElementDims() const
	{
		if (!boxDimensions)
		{
			pmlog_error("Trying to access box dims before set");
			throw Except<Exception>();
		}
		const auto marginDims = attrCache->margin.ToDimensions();
		return { *boxDimensions.width + marginDims.width, *boxDimensions.height + marginDims.height };
	}

	bool Element::IsLaidOut() const
	{
		return boxPosition && boxDimensions;
	}

	bool Element::IsDimensioned() const
	{
		return (bool)boxDimensions;
	}

	const std::vector<std::string>& Element::GetClasses() const
	{
		return classes;
	}

	bool Element::HasClass(const std::string& needle) const
	{
		return std::ranges::find(classes, needle) != classes.end();
	}

	void Element::FinalizeAsRoot(const DimensionsSpec& size, std::vector<std::shared_ptr<sty::Stylesheet>> sheets, Graphics& gfx)
	{
		sty::StyleProcessor sp{ std::move(sheets), sty::Stylesheet::MakeBase() };

		SetDimension(*size.width, FlexDirection::Row, sp, gfx);
		if (size.height)
		{
			SetDimension(*size.height, FlexDirection::Column, sp, gfx);
		}
		else
		{
			const auto calcSize = QueryLayoutConstraints(FlexDirection::Column, sp, gfx);
			SetDimension(calcSize.basis, FlexDirection::Column, sp, gfx);
		}
		SetPosition({ 0.f, 0.f }, sp, gfx);
	}

	void Element::CacheAttributes(sty::StyleProcessor& sp) const
	{
		if (!attrCache)
		{
			attrCache.emplace();
			attrCache->margin = sp.Resolve<at::margin>();
			attrCache->border = sp.Resolve<at::border>();
			attrCache->padding = sp.Resolve<at::padding>();
			attrCache->display = sp.Resolve<at::display>();
		}
	}

	//void Element::BuildClassMap()
	//{
	//	for (auto& pChild : childPtrs)
	//	{
	//		for (auto& cls : pChild->GetClasses())
	//		{
	//			classMap.insert({ cls, pChild });
	//		}
	//		pChild->BuildClassMap();
	//		classMap.merge(pChild->classMap);
	//	}
	//}

	void Element::AddChild(std::shared_ptr<Element> pChild)
	{
		childPtrs.push_back(std::move(pChild));
	}

	std::span<const std::shared_ptr<Element>> Element::GetChildren() const
	{
		return childPtrs;
	}

	std::span<std::shared_ptr<Element>> Element::GetChildren()
	{
		return childPtrs;
	}
}