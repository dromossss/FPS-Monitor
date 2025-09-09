#pragma once

namespace pwr::v
{
#ifndef VVV_GPU_TELEMETRY // system that reads power, temperature, etc. telemetry of graphics adapter devices
    inline constexpr bool gpu = false;
#else
    inline constexpr bool gpu = true;
#endif	
}