// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Selector.h"
#include "RawAttributeValue.h"
#include "Attributes.h"
#include "AttributeUtils.h"

namespace p2c::gfx::lay::sty
{
	class Stylesheet
	{
	public:
		Stylesheet(Selector sel);
		template<class A>
		typename A::ResolvedType Resolve() const
		{
			if (auto i = rawAttributes.find(A::key); i != rawAttributes.end())
			{
				return at::ResolveVariantFromRaw<typename A::ResolvedType>(i->second);
			}
			return {};
		}
		template<typename V>
		void InsertRaw(std::string key, V val)
		{
			static_assert(
				std::is_same_v<V, at::RawAttributeObjectValue> ||
				std::is_same_v<V, at::RawString> ||
				std::is_same_v<V, at::RawNumber> ||
				std::is_same_v<V, at::RawBool>,
				"Raw inserted attribute value was wrong type"
			);
			rawAttributes[key] = std::move(val);
		}
		template<typename A, typename V>
		void InsertRaw(V val)
		{
			static_assert(
				std::is_base_of_v<at::Attribute, A>,
				"Insert raw attribute template argument is not valid"
			);
			InsertRaw(A::key, val);
		}
		const Selector& GetSelector() const;
		// destination (this) sheet values take precedence
		void MergeFrom(const Stylesheet& other);
		static std::shared_ptr<Stylesheet> Make(Selector sel = {});
		static std::shared_ptr<Stylesheet> MakeBase();
		static std::shared_ptr<Stylesheet> MakeDefaultInherit();
		bool operator>(const Stylesheet& rhs) const;
	private:
		Selector selector;
		std::unordered_map<std::string, at::RawAttributeValue> rawAttributes;
	};
}