// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
struct VertexOut
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

VertexOut main(float2 pos : POSITION, float4 color : COLOR)
{
    VertexOut vo;
    vo.pos = float4(pos, 0.0f, 1.0f);
    vo.color = color;
    return vo;
}