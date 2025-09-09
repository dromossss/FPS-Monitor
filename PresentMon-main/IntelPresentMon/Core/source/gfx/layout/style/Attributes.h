// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <variant>
#include <string>
#include <Core/source/gfx/base/Geometry.h>
#include "SpecialAttributes.h"
#include "../Enums.h"
#include <Core/source/gfx/prim/Enums.h>

namespace p2c::gfx::lay::sty::at
{
	// monostate option needed to signal that a particular sheet did not have the attribute key
	template<class... T>
	using ResolvedT = std::variant<std::monostate, T...>;

	// TODO:? maybe move these into AttributeBases.h, and then remove Resolver dependency on this file
	struct Attribute {};
	struct ReadOnlyAttribute : Attribute {};

// OR macro needed because comma in the template argument list causes headaches
#define OR_ ,

// defines set of 4 attributes for skirt (border, margin, etc.)
#define XAT_DEF_SKIRT(tem, name) \
	tem(name##Left, ResolvedT<float OR_ at::Special::Inherit>, true, false, 0.); \
	tem(name##Top, ResolvedT<float OR_ at::Special::Inherit>, true, false, 0.); \
	tem(name##Right, ResolvedT<float OR_ at::Special::Inherit>, true, false, 0.); \
	tem(name##Bottom, ResolvedT<float OR_ at::Special::Inherit>, true, false, 0.)

#define XAT_ATTRIBUTE_LIST(tem) \
	tem(width, ResolvedT<float OR_ at::Special::Auto>, true, true, at::make::Special<at::Special::Auto>()) \
	tem(height, ResolvedT<float OR_ at::Special::Auto>, true, true, at::make::Special<at::Special::Auto>()) \
	tem(backgroundColor, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::White(0.))) \
	tem(borderColorLeft, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::Black())) \
	tem(borderColorTop, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::Black())) \
	tem(borderColorRight, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::Black())) \
	tem(borderColorBottom, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::Black())) \
	tem(flexAlignment, ResolvedT<FlexAlignment OR_ at::Special::Inherit>, true, false, at::make::Enum(FlexAlignment::Start)) \
	tem(flexJustification, ResolvedT<FlexJustification OR_ at::Special::Inherit>, true, false, at::make::Enum(FlexJustification::Start)) \
	tem(flexDirection, ResolvedT<FlexDirection OR_ at::Special::Inherit>, true, false, at::make::Enum(FlexDirection::Row)) \
	tem(flexGrow, ResolvedT<float OR_ at::Special::Inherit>, true, false, 0.) \
	tem(display, ResolvedT<Display>, true, false, at::make::Enum(Display::Visible)) \
	tem(textStyle, ResolvedT<prim::Style OR_ at::Special::Inherit>, true, false, at::make::Enum(prim::Style::Normal)) \
	tem(textWeight, ResolvedT<prim::Weight OR_ at::Special::Inherit>, true, false, at::make::Enum(prim::Style::Normal)) \
	tem(textJustification, ResolvedT<prim::Justification OR_ at::Special::Inherit>, true, false, at::make::Enum(prim::Justification::Left)) \
	tem(textAlignment, ResolvedT<prim::Alignment OR_ at::Special::Inherit>, true, false, at::make::Enum(prim::Alignment::Top)) \
	tem(textFont, ResolvedT<std::wstring OR_ at::Special::Inherit>, true, false, std::wstring(L"Verdana")) \
	tem(textSize, ResolvedT<float OR_ at::Special::Inherit>, true, false, 12.) \
	tem(textColor, ResolvedT<Color OR_ at::Special::Inherit>, true, false, at::make::Color(Color::Black())) \
	tem(graphType, ResolvedT<GraphType>, true, false, at::make::Enum(GraphType::Line)) \
	tem(graphGridColor, ResolvedT<Color>, true, false, at::make::Color(Color::FromBytes(47, 120, 190, 40))) \
	tem(graphLineColor, ResolvedT<Color>, true, false, at::make::Color(Color::FromBytes(100, 255, 255, 220))) \
	tem(graphFillColor, ResolvedT<Color>, true, false, at::make::Color(Color::FromBytes(57, 210, 250, 25))) \
	tem(graphHorizontalDivs, ResolvedT<int>, true, false, 40.) \
	tem(graphVerticalDivs, ResolvedT<int>, true, false, 4.) \
	tem(graphMinValueLeft, ResolvedT<float>, true, false, 0.) \
	tem(graphMaxValueLeft, ResolvedT<float>, true, false, 100.) \
	tem(graphMinValueRight, ResolvedT<float>, true, false, 0.) \
	tem(graphMaxValueRight, ResolvedT<float>, true, false, 100.) \
	tem(graphAutoscaleLeft, ResolvedT<bool>, true, false, false) \
	tem(graphAutoscaleRight, ResolvedT<bool>, true, false, false) \
	tem(graphAutoscaleCount, ResolvedT<bool>, true, false, false) \
	tem(graphTimeWindow, ResolvedT<float>, true, false, 5.) \
	tem(graphBinCount, ResolvedT<int>, true, false, 40.) \
	tem(graphMinCount, ResolvedT<int>, true, false, 0.) \
	tem(graphMaxCount, ResolvedT<int>, true, false, 120.) \
	tem(graphAntiAlias, ResolvedT<bool>, true, false, false) \
	XAT_DEF_SKIRT(tem, border) \
	XAT_DEF_SKIRT(tem, padding) \
	XAT_DEF_SKIRT(tem, margin)

#define DEF_ATT(name, resolved, simp, aut, def) \
struct name : Attribute \
{ \
	using ResolvedType = resolved; \
	static constexpr const char* key = #name; \
	static constexpr bool isSimplex = simp; \
	static constexpr bool isOptAuto = aut; \
};

	XAT_ATTRIBUTE_LIST(DEF_ATT)

#undef DEF_ATT
}