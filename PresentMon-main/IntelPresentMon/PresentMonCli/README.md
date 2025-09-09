# PresentMon CLI

The PresentMon CLI uses the PresentMon Service to report out ETW based frame metrics and sampled CPU/GPU telemetry. It shares many of the command line arguments as the original PresentMon application with additional arguments related to the sampled GPU/CPU telemetry.

## License

Copyright (C) 2017-2023 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Command line options

| Capture Target Options |                                                                                                                  |
| ---------------------- | ---------------------------------------------------------------------------------------------------------------- |
| `-A, --captureall`          | Record all processes (default).                                                                                  |
| `--process_name name`   | Record only processes with the provided exe name.  This argument can be repeated to capture multiple processes.  |
| `--exclude name`        | Don't record processes with the provided exe name.  This argument can be repeated to exclude multiple processes. |
| `--process_id id`       | Record only the process specified by ID.                                                                         |
| `--etl_file path`       | Consume events from an ETW log file instead of running processes.                                                |
| -i, --ignore_case        | Ignore case when matching a process name                                                                         |

| Output Options      |                                                                          |
| ------------------- | ------------------------------------------------------------------------ |
| `-o, --output_file path` | Write CSV output to the provided path.                            |
| `-s, --output_stdout`    | Write CSV output to STDOUT.                                        |
| `-m, --multi_csv`        | Create a separate CSV file for each captured process.               |
| `-v, --no_csv`           | Do not create any output file.                                      |

| Recording Options   |                                                                                                                                                 |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `--delay seconds`    | Wait for provided time before starting to record.                            |
| `--timed seconds`    | Stop recording after the provided amount of time.                                                                                               |
| `-d, --exclude_dropped`  | Exclude dropped presents from the csv output.                                                                                                   |

| GPU/CPU Options |                                                                                              |
|-----------------|----------------------------------------------------------------------------------------------|
| `--track_gpu_telemetry`   | Tracks GPU telemetry relating to power, temperature, frequency, clock speed, etc. |
| `--track_vram_telemetry`  | Tracks VRAM telemetry relating to power, temperature, frequency, etc.             |
| `--track_gpu_memory`      | Tracks GPU memory utilization                                                     |
| `--track_gpu_fan`          | Tracks GPU fanspeeds                                                              |
| `--track_gpu_psu`          | Tracks GPU PSU information                                                        |
| `--track_gpu_psu`          | Tracks GPU PSU information                                                        |
| `--track_perf_limit`       | Tracks flags denoting current reason for performance limitation                   |
| `--track_cpu_telemetry`   | Tracks CPU telemetry relating to power, temperature, frequency, clock speed, etc.  |
| `--track_powershare_bias` | Tracks powershare bias information.                                               |

| Beta Options              |                                                                                                         |
| ------------------------- | ------------------------------------------------------------------------------------------------------- |
| `--track_gpu`              | Tracks the duration of each process' GPU work performed between presents.  Not supported on Win7.       |
| `--track_gpu_video`        | Track the video encode/decode portion of GPU work separately from other engines. Not supported on Win7. |
| `--track_input`            | Tracks the time of keyboard/mouse clicks that were used by each frame.                                  |
| `-track_memory_residency` | Capture CPU time spent in memory residency and paging operations during each frame.                     |

| Internal Options      |                                                            |
| --------------------- | ---------------------------------------------------------- |
| `--track_queue_timers` | Capture Intel D3D11 driver producer/consumer queue timers. |
| `--track_cpu_gpu_sync` | Capture Intel D3D11 driver CPU/GPU syncs.                  |
| `--track_shader_compilation` | Capture Intel D3D11 driver shader compilation.       |

Note: internal options require a release-internal driver, with the GfxEvents manifest installed from its corresponding TestTools package. Some options may also require specific driver feature branches.

## Comma-separated value (CSV) file output

### CSV file names

By default, PresentMon creates a CSV file named "PresentMon-\<Time>.csv", where "\<Time>" is the creation time in ISO 8601 format.  To specify your own output location, use the `-output_file PATH` command line argument.

If `-multi_csv` is used, then one CSV is created for each process captured and "-\<ProcessName>-\<ProcessId>" is appended to the file name.

### CSV columns

| Column Header              | Data Description                                                                                                                                                                                                                                                                                                                                         |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| *Application*              | The name of the process that called Present().                                                                                                                                                                                                                                                                                                           |
| *ProcessID*                | The process ID of the process that called Present().                                                                                                                                                                                                                                                                                                     |
| *SwapChainAddress*         | The address of the swap chain that was presented into.                                                                                                                                                                                                                                                                                                   |
| *Runtime*                  | The runtime used to present (e.g., D3D9 or DXGI).                                                                                                                                                                                                                                                                                                        |
| *SyncInterval*             | The sync interval provided by the application in the Present() call. This value may be modified later by the driver, e.g., based on control panel overrides.                                                                                                                                                                                             |
| *PresentFlags*             | Flags used in the Present() call.                                                                                                                                                                                                                                                                                                                        |
| *Dropped*                  | Whether the frame was dropped (1) or displayed (0).  Note, if dropped, *msUntilDisplayed* will be 0.                                                                                                                                                                                                                                                     |
| *TimeInSeconds*            | The time of the Present() call, in seconds, relative to when the PresentMon started recording.                                                                                                                                                                                                                                                           |
| *InPresentAPI[ms]*         | The time spent inside the Present() call, in milliseconds.                                                                                                                                                                                                                                                                                               |
| *BetweenPresents[ms]*      | The time between this Present() call and the previous one, in milliseconds.                                                                                                                                                                                                                                                                              |
| *AllowsTearing*            | Whether tearing is possible (1) or not (0).<br>This column is not available when `-no_track_display` is used.                                                                                                                                                                                                                                          |
| *PresentMode*              | The presentation mode used by the system for this Present().  See the table below for more details.<br>This column is not available when `-no_track_display` is used.                                                                                                                                                                                  |
| *UntilRenderComplete[ms]*  | The time between the Present() call and when GPU work for this frame completed, in milliseconds.<br>This column is not available when `-no_track_display` is used.                                                                                                                                                                                     |
| *UntilDisplayed[ms]*       | The time between the Present() call and when the frame was displayed, in milliseconds.<br>This column is not available when `-no_track_display` is used.                                                                                                                                                                                               |
| *BetweenDisplayChange[ms]* | How long the previous frame was displayed before this Present() was displayed, in milliseconds.<br>This column is not available when `-no_track_display` is used.                                                                                                                                                                                      |
| *QPCTime*                  | The time of the Present() call, as a [performance counter value](https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter).<br>This column is only available when `-qpc_time` or `-qpc_time_s` are used.  When `-qpc_time_s` is used, the value is converted to seconds by dividing by the counter frequency. |

Using `--track_gpu` or `--track_gpu_video` will add the following columns:

| Column Header                         | Data Description                                                                            |
| ------------------------------------- | ------------------------------------------------------------------------------------------- |
| *UntilRenderStart[ms]*                | The time between the Present() call and when GPU work for this frame started, in milliseconds.  Note that rendering for a frame can start before the Present() call, so this value can be negative. |
| *GPUActive[ms]*, *GPUVideoActive[ms]* | The total duration the GPU was working on this frame, in milliseconds.  Time is counted whenever at least one engine is executing work from the target process. When `-track_gpu_video` is used, then the *msGPUVideoActive* column is added showing the duration of work on the GPU's video encode and/or decode engines and, in this case, the video encode/decode work is not included in *msGPUActive*. |

Using `--track_input` will add the following columns:

| Column Header    | Data Description                                                                            |
| ---------------- | ------------------------------------------------------------------------------------------- |
| *SinceInput[ms]* | The time between the Present() call and the earliest keyboard or mouse interaction that contributed to this frame.  For frames where *msSinceInput* is non-zero, `msSinceInput + msUntilDisplayed` can be used as a measure of the latency between user input and the display of the resulting rendered frame.  Note, however, that this is just the software-visible subset of the full input-to-photon latency and doesn't include:<br>&bull; time spent processing input in the keyboard/controller hardware or drivers (typically a fixed additional overhead),<br>&bull; time spent processing the output in the display hardware or drivers (typically a fixed additional overhead), and<br>&bull; a combination of display blanking interval and scan time (which varies, depending on timing and tearing). |

Using `--track_queue_timers` will add the following columns:

| Column Header                | Data Description                                                                            |
| ---------------------------- | ------------------------------------------------------------------------------------------- |
| *StalledQueueFull[ms]*       | How long the producer thread was stalled on a full queue, in milliseconds.                  |
| *StalledQueueEmpty[ms]*      | How long the consumer thread was stalled on an empty queue, in milliseconds.                |
| *WaitingQueueSync[ms]*       | How long the driver waited for the queue to empty due to synchronization, in milliseconds.  |
| *WaitingQueueDrain[ms]*      | How long the driver waited for the queue to drain, in milliseconds.                         |
| *WaitingFence[ms]*           | How long the driver waited for consumer fences, in milliseconds.                            |
| *WaitingFenceSubmission[ms]* | How long the driver waited for fences submitted to the GPU, in milliseconds.                |
| *BetweenProducerPresents[ms]*  | The time between the producer thread processing this Present and the previous Present, in milliseconds. |
| *BetweenConsumerEvents[ms]*  | The time between the consumer thread processing this Present and the previous Present, in milliseconds. |

Using `--track_cpu_gpu_sync` will add the following columns:

| Column Header           | Data Description                                                                 |
| ----------------------- | -------------------------------------------------------------------------------- |
| *WaitingSyncObject[ms]* | How much time was spent waiting for a sync object from the CPU, in milliseconds. |
| *WaitingQueryData[ms]*  | How much time the driver spent polling for query data, in milliseconds.          |

Using `--track_shader_compilation` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *WaitingDrawTimeCompilation[ms]*   | How much time was spent waiting for shader compilation on draw, in milliseconds.   |
| *WaitingCreateTimeCompilation[ms]* | How much time was spent waiting for shader compilation on create, in milliseconds. |

Using `--track_memory_residency` will add the following columns:

| Column Header       | Data Description                                                    |
| ------------------- | ------------------------------------------------------------------- |
| *InMakeResident[ms]*  | How much time was spent inside MakeResident calls, in milliseconds. |
| *InPagingPackets[ms]* | How much time was spent exectuing Paging packets, in milliseconds.  |

Using `--track_gpu_telemetry` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *GPUPower[W]*   | GPU power usage in W.   |
| *GPUSustainedPowerLimit[W]* | Sustained GPU power limit in W. |
| *GPUVoltage[V]*   | Voltage feeding the GPU chip in V.   |
| *GPUFrequency[MHz]* | GPU chip frequency in MHz. |
| *GPUTemperature[ms]*   | GPU chip temperature in C.   |
| *GPUUtilization[%]* | Percentage utilization of the GPU. |
| *GPURenderComputeUtilization[%]*   | Percentage utilization of the 3D and Compute blocks in the GPU.   |
| *GPUMediaUtilization[%]* | Percentage utilization of the media blocks in the GPU. |

Using `--track_vram_telemetry` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *VRAMPower[W]*   | Memory module power usage in W.   |
| *VRAMVoltage[V]* | Voltage feeding the memory modules in W. |
| *VRAMFrequency[MHz]*   | Memory module frequency in MHz.   |
| *VRAMEffectiveFrequency[MHz]* | Effective data transfer rate the memory modules can sustain based on the current clock frequency in MHz. |
| *VRAMReadBandwidth[bps]*   | Current memory module read bandwidth in bytes per second.   |
| *VRAMWriteBandwidth[bps]* | Current memory module write bandwidth in bytes per second. |
| *VRAMTemperature[C]*   | Memory modules temperature in C.   |

Using `--track_gpu_memory` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *GPUMemTotalSize[B]*   | Total GPU memory size in bytes.   |
| *GPUMemUsed[B]* | Total GPU memory used in bytes. |
| *GPUMemMaxBandwidth[bps]*   | Max memory module write bandwidth in bytes per second.   |
| *GPUMemReadBandwidth[bps]* | Current memory module read bandwidth in bytes per second. |
| *GPUMemWriteBandwidth[bps]* | Current memory module write bandwidth in bytes per second.   |

Using `--track_gpu_fan` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *GPUFanSpeed0[rpm]*   | GPU fan speed in RPMs.   |
| *GPUFanSpeed1[rpm]*   | GPU fan speed in RPMs.   |
| *GPUFanSpeed2[rpm]*   | GPU fan speed in RPMs.   |
| *GPUFanSpeed3[rpm]*   | GPU fan speed in RPMs.   |
| *GPUFanSpeed4[rpm]*   | GPU fan speed in RPMs.   |

Using `--track_gpu_psu` will add the following columns:

| Column Header                      | Data Description                                                                   |
| -----------------------------------| ---------------------------------------------------------------------------------- |
| *PSUType0*   | Specfies the Power Supply Type (PCIE, Pin6, Pin8) at index 0.   |
| *PSUType1*   | Specfies the Power Supply Type (PCIE, Pin6, Pin8) at index 1.   |
| *PSUType2*   | Specfies the Power Supply Type (PCIE, Pin6, Pin8) at index 2.   |
| *PSUType3*   | Specfies the Power Supply Type (PCIE, Pin6, Pin8) at index 3.   |
| *PSUType4*   | Specfies the Power Supply Type (PCIE, Pin6, Pin8) at index 4.   |
| *PSUPower0[W]*   | Total energy consumed by the power source in W at index 0.   |
| *PSUPower1[W]*   | Total energy consumed by the power source in W at index 1.   |
| *PSUPower2[W]*   | Total energy consumed by the power source in W at index 2.   |
| *PSUPower3[W]*   | Total energy consumed by the power source in W at index 3.   |
| *PSUPower4[W]*   | Total energy consumed by the power source in W at index 4.   |
| *PSUVoltage0[V]*   | Voltage of the power source in V at index 0.   |
| *PSUVoltage1[V]*   | Voltage of the power source in V at index 1.   |
| *PSUVoltage2[V]*   | Voltage of the power source in V at index 2.   |
| *PSUVoltage3[V]*   | Voltage of the power source in V at index 3.   |
| *PSUVoltage4[V]*   | Voltage of the power source in V at index 4.   |

Using `--track_perf_limit_` will add the following columns:

| Column Header             | Data Description                                                                                                              |
| --------------------------| ----------------------------------------------------------------------------------------------------------------------------- |
| *GPUPowerLimited*         | Specifies if the GPU frequency is being throttled because the GPU chip is exceeding the maximum power limits.                 |
| *GPUTemperatureLimited*   | Specifies if the GPU frequency is being throttled because the GPU chip is exceeding the maximum temperature limits.           |
| *GPUCurrentLimited*       | Specifies if the GPU frequency is being throttled because the GPU chip is exceeding power supply current limits.              |
| *GPUVoltageLimited*       | Specifies if the GPU frequency is being throttled because the GPU chip is exceeding the voltage limits.                       |
| *GPUUtilizationLimited*   | Specifies if the GPU frequency has been lowered due to low GPU utilization.                                                   |
| *VRAMPowerLimited*        | Specifies if the memory frequency is being throttled because the memory modules are exceeding the maximum power limits.       |
| *VRAMTemperatureLimited*  | Specifies if the memory frequency is being throttled because the memory modules are exceeding the maximum temperature limits. |
| *VRAMCurrentLimited*      | Specifies if the memory frequency is being throttled because the GPU chip is exceeding the maximum power suppy limits.        |
| *VRAMVoltageLimited*      | Specifies if the memory frequency is being throttled because the GPU chip is exceeding the voltage limits.                    |
| *VRAMUtilizationLimited*  | Specifies if the memory frequency has been lowered due to low memory utilization.                                             |

Using `--track_cpu_telemetry` will add the following columns:

| Column Header       | Data Description                     |
| --------------------| -------------------------------------|
| *CPUUtilization[%]* | Percentage utilization of the CPU.   |
| *CPUPower[W]*       | CPU power usage in W.                |
| *CPUPowerLimit[W]*  | Sustained CPU power limit in W.      |
| *CPUTemperature[C]* | CPU temperature in C.                |
| *CPUFrequency[GHz]* |CPU frequency in GHz.                 |

Using `--track_powershare_bias_` will add the following columns:

| Column Header       | Data Description      |
| --------------------| ----------------------|
| *CPUBias*       | Current CPU Bias value.   |
| *GPUBias*       | Current GPU Bias value.   |

Note: The above values are the telemetry items supported by PresentMon. Not every graphics card will report all items. Reported telemetry items vary by graphics card and drivers.

The following values are used in the PresentMode column:

| PresentMode                           | Description                                                                                                                                                                                          |
| ------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Hardware: Legacy Flip                 | Indicates the app took ownership of the screen, and is swapping the displayed surface every frame.                                                                                                   |
| Hardware: Legacy Copy to front buffer | Indicates the app took ownership of the screen, and is copying new contents to an already-on-screen surface every frame.                                                                             |
| Hardware: Independent Flip            | Indicates the app does not have ownership of the screen, but is still swapping the displayed surface every frame.                                                                                    |
| Composed: Flip                        | Indicates the app is windowed, is using ["flip model" swapchains](https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model), and is sharing its surfaces with DWM to be composed. |
| Hardware Composed: Independent Flip   | Indicates the app is using ["flip model" swapchains](https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model), and has been granted a hardware overlay plane.                    |
| Composed: Copy with GPU GDI           | Indicates the app is windowed, and is copying contents into a surface that's shared with GDI.                                                                                                        |
| Composed: Copy with CPU GDI           | Indicates the app is windowed, and is copying contents into a dedicated DirectX window surface. GDI contents are stored separately, and are composed together with DX contents by the DWM.           |

For more information on the performance implications of these, see:

- https://www.youtube.com/watch?v=E3wTajGZOsA
- https://software.intel.com/content/www/us/en/develop/articles/sample-application-for-direct3d-12-flip-model-swap-chains.html

## Known issues

See [public GitHub Issues](https://github.com/GameTechDev/PresentMon/issues) or [internal GitHub Issues](https://github.com/GameTechDev/PresentMon/issues) for a current list of reported issues.

### Analyzing OpenGL and Vulkan applications

Applications that do not use D3D9 or DXGI APIs for presenting frames (e.g., as is typical with OpenGL or Vulkan applications) will report the following:

- *Runtime* = Other
- *SwapChainAddress* = 0
- *msInPresentAPI* = 0

In this case, *TimeInSeconds* will represent the first time the present is observed in the kernel, as opposed to the runtime, and therefore will be sometime after the application presented the frame (typically ~0.5ms).  Since *msUntilRenderComplete* and *msUntilDisplayed* are deltas from *TimeInSeconds*, they will be correspondingly smaller then they would have been if measured from application present.  *msBetweenDisplayChange* will still be correct, and *msBetweenPresents* should be correct on average.

### Tracking GPU work with Hardware-Accelerated GPU Scheduling enabled

On a system that uses Hardware-Accelerated GPU Scheduling (HWS), the GPU
execution metrics are less accurate than when HWS is
disabled resulting in *msUntilRenderStart*, *msUntilRenderComplete*,
*msGPUActive*, and *msGPUVideoActive* measurements that are later/larger than
they should be.  For example, in a GPU-bound scenario the frame's *msGPUActive*
may be reported ~0.5ms larger than the true GPU work duration, though the
specific amount of the inaccuracy will be workload- and GPU-dependent.

An improved solution is WIP.
