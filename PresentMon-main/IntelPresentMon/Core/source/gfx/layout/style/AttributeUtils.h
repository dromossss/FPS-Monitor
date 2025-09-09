// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "RawAttributeValue.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/Exception.h>
#include <Core/source/gfx/base/Geometry.h>
#include "SpecialAttributes.h"

namespace p2c::gfx::lay::sty::at
{
	using namespace ::pmon::util;

	template<class V, std::size_t I = 0>
	constexpr bool CanInherit()
	{
		using OutputType = std::variant_alternative_t<I, V>;
		if constexpr (std::is_same_v<Special::Inherit, OutputType>) {
			return true;
		}

		if constexpr (I + 1 < std::variant_size_v<V>) {
			return CanInherit<V, I + 1>();
		}
		else {
			return false;
		}
	}

	// tries to convert raw data to variant specified by Attribute
	template<class V, std::size_t I = 0>
	V ResolveVariantFromRaw(const RawAttributeValue& raw)
	{
		using OutputType = std::variant_alternative_t<I, V>;
		if constexpr (std::is_same_v<float, OutputType>) {
			if (std::holds_alternative<double>(raw)) return (float)std::get<double>(raw); // check if raw type matches output type
		}
		else if constexpr (std::is_same_v<int, OutputType>) {
			if (std::holds_alternative<double>(raw)) return (int)std::get<double>(raw);
		}
		else if constexpr (std::is_same_v<bool, OutputType>) {
			if (std::holds_alternative<bool>(raw)) return std::get<bool>(raw);
		}
		else if constexpr (std::is_same_v<RawString, OutputType>) {
			if (auto pStr = std::get_if<RawString>(&raw)) {
				if (pStr->size() > 0 && (pStr->front() == '@' || pStr->front() == '*')) {
					// value is special/enum so do not return string
				}
				else {
					return std::get<std::wstring>(raw);
				}
			}
		}
		else if constexpr (std::is_same_v<gfx::Color, OutputType>) {
			if (auto pObj = std::get_if<RawAttributeObjectValue>(&raw)) {
				return gfx::Color{
					(float)std::get<double>(pObj->at("r")) / float(255),
					(float)std::get<double>(pObj->at("g")) / float(255),
					(float)std::get<double>(pObj->at("b")) / float(255),
					(float)std::get<double>(pObj->at("a")),
				};
			}
		}
		else if constexpr (std::is_base_of_v<Special::Base, OutputType>) { // check for special value
			if (std::holds_alternative<std::wstring>(raw) && std::get<std::wstring>(raw) == OutputType::key)
				return std::variant_alternative_t<I, V>{};
		}
		else if constexpr (std::is_enum_v<OutputType>) { // check for enum output type
			if (auto ps = std::get_if<std::wstring>(&raw)) {
				if (ps->size() > 0 && ps->front() == L'*') {
					return EnumRegistry<OutputType>::ToEnum(ps->substr(1));
				}
			}
		}

		if constexpr (I + 1 < std::variant_size_v<V>) {
			return ResolveVariantFromRaw<V, I + 1>(raw);
		}
		else {
			pmlog_error("failed to resolve raw style attribute");
			throw Except<Exception>();
		}
	}
}