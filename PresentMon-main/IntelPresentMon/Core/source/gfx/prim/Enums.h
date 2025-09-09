// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <Core/source/gfx/layout/EnumUtils.h>

namespace p2c::gfx::prim
{
	#define XEN_LIST_PRIM_STYLE(X_) \
		X_(Normal) \
		X_(Italic) \
		X_(Oblique)

	XEN_GENERATE_ENUM(Style, XEN_LIST_PRIM_STYLE)


	#define XEN_LIST_PRIM_WEIGHT(X_) \
		X_(Normal) \
		X_(Bold) \
		X_(ExtraBold) \
		X_(Medium) \
		X_(Light) \
		X_(UltraLight)

	XEN_GENERATE_ENUM(Weight, XEN_LIST_PRIM_WEIGHT)


	#define XEN_LIST_PRIM_JUSTIFICATION(X_) \
		X_(Left) \
		X_(Right) \
		X_(Center) \
		X_(Full)

	XEN_GENERATE_ENUM(Justification, XEN_LIST_PRIM_JUSTIFICATION)


	#define XEN_LIST_PRIM_ALIGNMENT(X_) \
		X_(Top) \
		X_(Center) \
		X_(Bottom)

	XEN_GENERATE_ENUM(Alignment, XEN_LIST_PRIM_ALIGNMENT)
}

namespace p2c::gfx::lay
{
	XEN_GENERATE_REGISTRY(prim::Style, XEN_LIST_PRIM_STYLE)
	XEN_GENERATE_REGISTRY(prim::Weight, XEN_LIST_PRIM_WEIGHT)
	XEN_GENERATE_REGISTRY(prim::Justification, XEN_LIST_PRIM_JUSTIFICATION)
	XEN_GENERATE_REGISTRY(prim::Alignment, XEN_LIST_PRIM_ALIGNMENT)
}