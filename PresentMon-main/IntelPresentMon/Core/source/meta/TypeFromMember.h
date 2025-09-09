// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once

namespace p2c::meta
{
    template <typename T> struct MemberPointerInfo;

    template <typename S, typename M>
    struct MemberPointerInfo<M S::*> {
        using StructType = S;
        using MemberType = M;
    };
}