// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Stylesheet.h"
#include "RawAttributeHelpers.h"

namespace p2c::gfx::lay::sty
{
	Stylesheet::Stylesheet(Selector sel)
		:
		selector{ std::move(sel) }
	{}
	const Selector& Stylesheet::GetSelector() const
	{
		return selector;
	}
	// destination (this) sheet values take precedence
	void Stylesheet::MergeFrom(const Stylesheet& other)
	{
		rawAttributes.insert(other.rawAttributes.begin(), other.rawAttributes.end());
	}
	std::shared_ptr<Stylesheet> Stylesheet::Make(Selector sel)
	{
		return std::make_shared<Stylesheet>(std::move(sel));
	}
	std::shared_ptr<Stylesheet> Stylesheet::MakeBase()
	{
		auto pBase = std::make_shared<Stylesheet>(Selector{});

#define DEF_ATT(name, resolved, simp, aut, def) \
pBase->InsertRaw<at::name>(def);

		XAT_ATTRIBUTE_LIST(DEF_ATT)

#undef DEF_ATT

		return pBase;
	}
	// TODO: move this sheet from compiler stack into resolver above base sheet?
	std::shared_ptr<Stylesheet> Stylesheet::MakeDefaultInherit()
	{
		auto pInherit = std::make_shared<Stylesheet>(Selector{});
		const auto inherit_v = at::make::Special<at::Special::Inherit>();
		pInherit->InsertRaw<at::borderColorLeft>(inherit_v);
		pInherit->InsertRaw<at::borderColorTop>(inherit_v);
		pInherit->InsertRaw<at::borderColorRight>(inherit_v);
		pInherit->InsertRaw<at::borderColorBottom>(inherit_v);
		pInherit->InsertRaw<at::textStyle>(inherit_v);
		pInherit->InsertRaw<at::textWeight>(inherit_v);
		pInherit->InsertRaw<at::textFont>(inherit_v);
		pInherit->InsertRaw<at::textSize>(inherit_v);
		pInherit->InsertRaw<at::textColor>(inherit_v);
		return pInherit;
	}
	bool Stylesheet::operator>(const Stylesheet& rhs) const
	{
		return selector > rhs.selector;
	}
}