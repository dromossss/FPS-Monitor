// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <optional>
#include <limits>
#include "../base/Geometry.h"

namespace p2c::gfx::lay
{
	// DimensionsSpec is used to hold explicitly specified element dimensions
	// empty optional means that the dimension should be auto-sized
	struct DimensionsSpec
	{
		std::optional<float> width;
		std::optional<float> height;
		operator bool() const { return width && height; }
		template<typename S>
		operator DimensionsT<S>() const
		{
			return { S(*width), S(*height) };
		}
		template<typename S>
		DimensionsSpec(const DimensionsT<S>& dims) : width{ (float)dims.width }, height{ (float)dims.height } {}
		DimensionsSpec() = default;
		DimensionsSpec(std::optional<float> width_, std::optional<float> height_ = {}) : width{ width_ }, height{ height_ } {}
		bool operator==(const DimensionsSpec& rhs) const { return width == rhs.width && height == rhs.height; }
	};

	// LayoutConstraints are communicated from child to FlexElement parent to
	// enable the parent to size the child and distribute space
	struct LayoutConstraints
	{
		float min = 0.f;
		float max = std::numeric_limits<float>::max();
		float basis = 0.f;
		float flexGrow = 0.f;
	};
}