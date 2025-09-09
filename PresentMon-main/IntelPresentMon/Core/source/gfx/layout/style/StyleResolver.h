// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <vector>
#include <memory>
#include "Stylesheet.h"

namespace p2c::gfx::lay::sty
{
	class StyleResolver
	{
	public:
		StyleResolver() = default;
		StyleResolver(std::shared_ptr<Stylesheet> base)
		{
			SetBaseSheet(std::move(base));
		}
		void PushAppliedSheet(std::shared_ptr<Stylesheet> sheet)
		{
			sheetStack.push_back(std::move(sheet));
		}
		void PopAppliedSheet()
		{
			sheetStack.pop_back();
		}
		void SetBaseSheet(std::shared_ptr<Stylesheet> sheet)
		{
			pBaseSheet = std::move(sheet);
		}
		template<class A>
		auto Resolve() const
		{
			// if ReadOnly attribute
			if constexpr (std::is_base_of_v<at::ReadOnlyAttribute, A>)
			{
				return A::Resolve(*this);
			}
			// if guaranteed to resolve to a single type, just return that type (and not variant)
			else if constexpr (A::isSimplex && !A::isOptAuto)
			{
				return std::get<1>(Resolve_<A>());
			}
			// if guaranteed to resolve to either a single type or @auto, return optional (empty means @auto)
			else if constexpr (A::isSimplex && A::isOptAuto)
			{
				auto resolved = Resolve_<A>();
				using OptType = std::optional<std::variant_alternative_t<1, decltype(resolved)>>;
				if (std::holds_alternative<at::Special::Auto>(resolved))
				{
					return OptType{};
				}
				else
				{
					return OptType{ std::get<1>(resolved) };
				}
			}
			else
			{
				return Resolve_<A>();
			}
		}
	private:
		const Stylesheet* Cur_() const
		{
			return sheetStack.back().get();
		}
		template<class A>
		typename A::ResolvedType Resolve_() const
		{
			// try applied styles
			if (auto attributeValue = Cur_()->Resolve<A>();
				!std::holds_alternative<std::monostate>(attributeValue))
			{
				if constexpr (at::CanInherit<typename A::ResolvedType>())
				{
					// inherit from parent
					if (std::holds_alternative<at::Special::Inherit>(attributeValue))
					{
						return ResolveInherited_<A>();
					}
				}
				// applied style found
				return attributeValue;
			}
			// no match in applied style return base style
			// (base is not allowed to hold any Inherit values)
			return pBaseSheet->Resolve<A>();
		}
		template<class A>
		typename A::ResolvedType ResolveInherited_() const
		{
			for (auto i = std::next(sheetStack.rbegin()); i != sheetStack.rend(); i++)
			{
				if (auto attributeValue = (*i)->Resolve<A>();
					!std::holds_alternative<std::monostate>(attributeValue))
				{
					if constexpr (at::CanInherit<typename A::ResolvedType>())
					{
						// inherit from next parent up
						if (std::holds_alternative<at::Special::Inherit>(attributeValue))
						{
							continue;
						}
					}
					// inherited style found
					return attributeValue;
				}
				return pBaseSheet->Resolve<A>();
			}
			return pBaseSheet->Resolve<A>();
		}
		// member data
		std::vector<std::shared_ptr<Stylesheet>> sheetStack;
		std::shared_ptr<Stylesheet> pBaseSheet;
	};
}