// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once

// macro to transform parameter list with type + parameter name
#define NVW_ARGS_0()
#define NVW_ARGS_2(t1,v1) t1 v1
#define NVW_ARGS_4(t1,v1,t2,v2) t1 v1, t2 v2
#define NVW_ARGS_6(t1,v1,t2,v2,t3,v3) t1 v1, t2 v2, t3 v3
#define NVW_ARGS_8(t1,v1,t2,v2,t3,v3,t4,v4) t1 v1, t2 v2, t3 v3, t4 v4
#define NVW_ARGS_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5) t1 v1, t2 v2, t3 v3, t4 v4, t5 v5
#define NVW_ARGS_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6) t1 v1, t2 v2, t3 v3, t4 v4, t5 v5, t6 v6
#define NVW_ARGS_N(_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0,N,...) NVW_ARGS##N
#define NVW_ARGS(...) NVW_ARGS_N(__VA_ARGS__,_12,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_0,_0)(__VA_ARGS__)

// macro to transform parameter list with parameter name only
#define NVW_NAMES_0()
#define NVW_NAMES_2(t1,v1) v1
#define NVW_NAMES_4(t1,v1,t2,v2) v1, v2
#define NVW_NAMES_6(t1,v1,t2,v2,t3,v3) v1, v2, v3
#define NVW_NAMES_8(t1,v1,t2,v2,t3,v3,t4,v4) v1, v2, v3, v4
#define NVW_NAMES_10(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5) v1, v2, v3, v4, v5
#define NVW_NAMES_12(t1,v1,t2,v2,t3,v3,t4,v4,t5,v5,t6,v6) v1, v2, v3, v4, v5, v6
#define NVW_NAMES_N(_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0,N,...) NVW_NAMES##N
#define NVW_NAMES(...) NVW_NAMES_N(__VA_ARGS__,_12,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_0,_0)(__VA_ARGS__)