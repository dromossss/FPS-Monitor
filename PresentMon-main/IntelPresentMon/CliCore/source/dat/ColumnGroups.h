#pragma once

#define COLUMN_GROUP_LIST \
X_(track_gpu, "Duration of each process' GPU work performed between presents. No Win7 support.") \
X_(track_gpu_video, "Duration of each process' video GPU work performed between presents. No Win7 support.") \
X_(track_input, "Time of keyboard/mouse clicks that were used by each frame.") \
X_(track_gpu_telemetry, "GPU telemetry relating to power, temperature, frequency, clock speed, etc.") \
X_(track_vram_telemetry, "VRAM telemetry relating to power, temperature, frequency, etc.") \
X_(track_gpu_memory, "GPU memory utilization.") \
X_(track_gpu_fan, "GPU fanspeeds.") \
X_(track_gpu_psu, "GPU PSU information.") \
X_(track_perf_limit, "Flags denoting current reason for performance limitation.") \
X_(track_cpu_telemetry, "CPU telemetry relating to power, temperature, frequency, clock speed, etc.")