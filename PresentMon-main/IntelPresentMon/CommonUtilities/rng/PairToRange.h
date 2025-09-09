// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <ranges>

namespace pmon::util::rng
{
    struct pair_to_range_t {
        template <typename I>
        friend constexpr auto operator|(std::pair<I, I> const& pr, pair_to_range_t) {
            return std::ranges::subrange(pr.first, pr.second);
        }
    };
    inline constexpr pair_to_range_t PairToRange{};
}