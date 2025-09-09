// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../Graphics.h"
#include "style/Stylesheet.h"
#include <optional>
#include <memory>
#include <span>
#include <limits>
#include <vector>
#include <unordered_map>
#include "Geometry.h"

namespace p2c::gfx::prim
{
	class BrushPrimitive;
	class RectPrimitive;
	class DrawablePrimitive;
}

// element is the building block of a layout
// think of it like a <div> in html

namespace p2c::gfx::lay
{
	namespace sty
	{
		class StyleProcessor;
	}

	class Element
	{
	public:
		Element(std::vector<std::string> classes = {});
		virtual ~Element();
		void SetDimension(float dimension, FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx);
		void SetPosition(const Vec2& pos, sty::StyleProcessor& sp, Graphics& gfx);
		void AddChild(std::shared_ptr<Element> pChild);
		std::span<const std::shared_ptr<Element>> GetChildren() const;
		std::span<std::shared_ptr<Element>> GetChildren();
		void Draw(Graphics& gfx) const;
		LayoutConstraints QueryLayoutConstraints(FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx) const;
		Rect GetElementRect() const;
		Rect GetBoxRect() const;
		Rect GetBackgroundRect() const;
		Rect GetContentRect() const;
		Dimensions GetElementDims() const;
		bool IsLaidOut() const;
		bool IsDimensioned() const;
		const std::vector<std::string>& GetClasses() const;
		bool HasClass(const std::string& needle) const;
		void FinalizeAsRoot(const DimensionsSpec& size, std::vector<std::shared_ptr<sty::Stylesheet>> sheets, Graphics& gfx);
		// void BuildClassMap();
	protected:
		constexpr static float unlimited = std::numeric_limits<float>::max();
		// element type-specific virtual implementation functions
		virtual LayoutConstraints QueryLayoutConstraints_(std::optional<float> width, sty::StyleProcessor& sp, Graphics& gfx) const = 0;
		virtual void SetDimension_(float dimension, FlexDirection dir, sty::StyleProcessor& sp, Graphics& gfx) = 0;
		virtual void SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx) = 0;
		virtual void Draw_(Graphics& gfx) const = 0;
	private:
		// functions
		void CacheAttributes(sty::StyleProcessor& sp) const;
		// data
		std::vector<std::string> classes;
		std::vector<std::shared_ptr<Element>> childPtrs;
		// std::unordered_multimap<std::string, std::shared_ptr<Element>> classMap;
		// calculated/cached specifications
		DimensionsSpec boxDimensions;
		std::optional<Vec2> boxPosition;
		// we are cacheing style info of margin/border/padding
		// it's hard to know where this is first accessed, so there is an init function
		// called from both SetDimension and QueryLayoutConstraints, this flag tells if
		// cacheing was already done
		struct AttributeCache
		{
			Skirt margin;
			Skirt border;
			Skirt padding;
			Display display = Display::Visible;
		};
		mutable std::optional<AttributeCache> attrCache;

		// retained primitives
		std::unique_ptr<prim::DrawablePrimitive> pBorder;
		std::unique_ptr<prim::RectPrimitive> pBackground;
	};
}
