// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "StyleResolver.h"
#include "../Geometry.h"


namespace p2c::gfx::lay::sty::at
{
	template<typename Left, typename Top, typename Right, typename Bottom>
	Skirt ResolveSkirt(const StyleResolver& resolver)
	{
		return {
			resolver.Resolve<Left>(),
			resolver.Resolve<Top>(),
			resolver.Resolve<Right>(),
			resolver.Resolve<Bottom>(),
		};
	}

#define X_DEF_SKIRT(name) \
struct name : ReadOnlyAttribute \
{ \
	static Skirt Resolve(const StyleResolver& resolver) \
	{ \
		return ResolveSkirt<at::name##Left, at::name##Top, at::name##Right, at::name##Bottom>(resolver); \
	} \
}

	X_DEF_SKIRT(margin);
	X_DEF_SKIRT(border);
	X_DEF_SKIRT(padding);

#undef X_DEF_SKIRT

	struct size : ReadOnlyAttribute
	{
		static DimensionsSpec Resolve(const StyleResolver& resolver)
		{
			return { resolver.Resolve<at::width>(), resolver.Resolve<at::height>() };
		}
	};
}