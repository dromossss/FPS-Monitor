:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@"%~dp0..\..\awk.exe" -f "%~dp0metrics.awk" "%~dp0..\..\..\IntelPresentMon\metrics.csv" >"%~dp0..\..\..\build\obj\generated\metadata\EnumMetric.h"
