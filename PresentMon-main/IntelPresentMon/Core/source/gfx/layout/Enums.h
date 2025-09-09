// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "EnumUtils.h"

namespace p2c::gfx::lay
{
	#define XEN_LIST_FLEX_ALIGNMENT(X_) \
		X_(Start) \
		X_(Center) \
		X_(End) \
		X_(Stretch)

	XEN_GENERATE_ENUM(FlexAlignment, XEN_LIST_FLEX_ALIGNMENT)
	XEN_GENERATE_REGISTRY(FlexAlignment, XEN_LIST_FLEX_ALIGNMENT)


	#define XEN_LIST_FLEX_JUSTIFICATION(X_) \
		X_(Start) \
		X_(Center) \
		X_(End) \
		X_(Between) \
		X_(Around) \
		X_(Even)

	XEN_GENERATE_ENUM(FlexJustification, XEN_LIST_FLEX_JUSTIFICATION)
	XEN_GENERATE_REGISTRY(FlexJustification, XEN_LIST_FLEX_JUSTIFICATION)


	#define XEN_LIST_FLEX_DIRECTION(X_) \
		X_(Row) \
		X_(Column)

	XEN_GENERATE_ENUM(FlexDirection, XEN_LIST_FLEX_DIRECTION)
	XEN_GENERATE_REGISTRY(FlexDirection, XEN_LIST_FLEX_DIRECTION)


	#define XEN_LIST_GRAPH_TYPE(X_) \
		X_(Line) \
		X_(Histogram)

	XEN_GENERATE_ENUM(GraphType, XEN_LIST_GRAPH_TYPE)
	XEN_GENERATE_REGISTRY(GraphType, XEN_LIST_GRAPH_TYPE)


	#define XEN_LIST_DISPLAY(X_) \
		X_(Visible) \
		X_(Invisible) \
		X_(None)

	XEN_GENERATE_ENUM(Display, XEN_LIST_DISPLAY)
	XEN_GENERATE_REGISTRY(Display, XEN_LIST_DISPLAY)


	#define XEN_LIST_AXIS_AFFINITY(X_) \
		X_(Left) \
		X_(Right)

	XEN_GENERATE_ENUM(AxisAffinity, XEN_LIST_AXIS_AFFINITY)
	XEN_GENERATE_REGISTRY(AxisAffinity, XEN_LIST_AXIS_AFFINITY)
}