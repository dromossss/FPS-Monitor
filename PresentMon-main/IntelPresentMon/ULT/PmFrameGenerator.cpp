#include "PmFrameGenerator.h"

PmFrameGenerator::PmFrameGenerator(const FrameParams& frame_params)
    : app_name_{frame_params.app_name.value_or("test_app")},
      process_id_{frame_params.process_id.value_or(10)},
      runtime_{frame_params.runtime.value_or(Runtime::D3D9)},
      sync_interval_{frame_params.sync_interval.value_or(0)},
      present_flags_{frame_params.present_flags.value_or(512)},
      percent_dropped_{frame_params.percent_dropped.value_or(0.)},
      in_present_api_ms_{frame_params.in_present_api_ms.value_or(1.3707)},
      in_present_api_variation_ms_{
          frame_params.in_present_api_variation_ms.value_or(1.3)},
      between_presents_ms_{frame_params.between_presents_ms.value_or(7.15)},
      between_presents_variation_ms_{
          frame_params.between_presents_variation_ms.value_or(0.15)},
      percent_tearing_{frame_params.percent_tearing.value_or(0.)},
      present_mode_{frame_params.present_mode.value_or(
          PresentMode::Hardware_Independent_Flip)},
      until_render_complete_ms_{
          frame_params.until_render_complete_ms.value_or(6.2663)},
      until_render_complete_variation_ms_{
          frame_params.until_render_complete_variation_ms.value_or(0.)},
      until_displayed_ms_{frame_params.until_displayed_ms.value_or(11.35)},
      until_displayed_variation_ms_{
          frame_params.until_displayed_ms.value_or(0.2)},
      until_display_change_ms_{
          frame_params.until_display_change_ms.value_or(7.15)},
      until_display_change_variation_ms_{
          frame_params.until_display_change_variation_ms.value_or(0.15)},
      until_render_start_ms_{
          frame_params.until_render_start_ms.value_or(-10.96)},
      until_render_start_variation_ms_{
          frame_params.until_render_start_variation_ms.value_or(0.)},
      qpc_time_{frame_params.qpc_time.value_or(0)},
      since_input_ms_{frame_params.since_input_ms.value_or(15.2)},
      since_input_variation_ms_{frame_params.since_input_ms.value_or(3.)},
      gpu_active_ms_{frame_params.gpu_active_ms.value_or(7.05)},
      gpu_active_variation_ms_{
          frame_params.gpu_active_variation_ms.value_or(0.0)},
      gpu_video_active_ms_{frame_params.gpu_video_active_ms.value_or(7.05)},
      gpu_video_active_variation_ms_{
          frame_params.gpu_video_active_variation_ms.value_or(0.0)},
      gpu_power_w_{frame_params.gpu_power_w.value_or(135.34)},
      gpu_power_variation_w_{frame_params.gpu_power_variation_w.value_or(37.)},
      gpu_sustained_power_limit_w_{
          frame_params.gpu_sustained_power_limit_w.value_or(190.)},
      gpu_sustained_power_limit_variation_w_{
          frame_params.gpu_sustained_power_limit_variation_w.value_or(0.)},
      gpu_voltage_v_{frame_params.gpu_voltage_v.value_or(1.032)},
      gpu_voltage_variation_v_{
          frame_params.gpu_voltage_variation_v.value_or(0.)},
      gpu_frequency_mhz_{frame_params.gpu_frequency_mhz.value_or(2400.)},
      gpu_frequency_variation_mhz_{
          frame_params.gpu_frequency_variation_mhz.value_or(0.)},
      gpu_temp_c_{frame_params.gpu_temp_c.value_or(62.7)},
      gpu_temp_variation_c_{
          frame_params.gpu_frequency_variation_mhz.value_or(1.01)},
      gpu_util_percent_{frame_params.gpu_util_percent.value_or(98.3)},
      gpu_util_variation_percent_{
          frame_params.gpu_util_variation_percent.value_or(5.9)},
      gpu_render_compute_util_percent_{
          frame_params.gpu_render_compute_util_percent.value_or(96.4)},
      gpu_render_compute_util_variation_percent_{
          frame_params.gpu_render_compute_util_variation_percent.value_or(7.)},
      gpu_media_util_percent_{frame_params.gpu_media_util_percent.value_or(0.)},
      gpu_media_util_variation_percent_{
          frame_params.gpu_media_util_variation_percent.value_or(0.)},
      vram_power_w_{frame_params.vram_power_w.value_or(0.)},
      vram_power_variation_w_{frame_params.vram_power_variation_w.value_or(0.)},
      vram_voltage_v_{frame_params.vram_voltage_v.value_or(0.)},
      vram_voltage_variation_v_{
          frame_params.vram_voltage_variation_v.value_or(0.)},
      vram_frequency_mhz_{frame_params.vram_frequency_mhz.value_or(2000.)},
      vram_frequency_variation_mhz_{
          frame_params.vram_frequency_variation_mhz.value_or(0.)},
      vram_effective_frequency_gbps_{
          frame_params.vram_effective_frequency_gbps.value_or(16000.)},
      vram_effective_frequency_variation_gbps_{
          frame_params.vram_effective_frequency_variation_gbps.value_or(0.)},
      vram_temp_c_{frame_params.vram_temp_c.value_or(71.)},
      vram_temp_variation_c_{frame_params.vram_temp_variation_c.value_or(1.2)},
      gpu_mem_total_size_b_{
          frame_params.gpu_mem_total_size_b.value_or(8589934592)},
      gpu_mem_total_size_variation_b_{
          frame_params.gpu_mem_total_size_variation_b.value_or(0)},
      gpu_mem_used_b_{frame_params.gpu_mem_total_size_b.value_or(2192377540)},
      gpu_mem_used_variation_b_{
          frame_params.gpu_mem_total_size_variation_b.value_or(3016908)},
      gpu_mem_max_bw_gbps_{
          frame_params.gpu_mem_max_bw_gbps.value_or(512000000000)},
      gpu_mem_max_bw_variation_gbps_{
          frame_params.gpu_mem_max_bw_variation_gbps.value_or(0)},
      gpu_mem_read_bw_bps_{
          frame_params.gpu_mem_read_bw_bps.value_or(55754930711)},
      gpu_mem_read_bw_variation_bps_{
          frame_params.gpu_mem_read_bw_variation_bps.value_or(4938578793)},
      gpu_mem_write_bw_bps_{
          frame_params.gpu_mem_max_bw_gbps.value_or(35272691238)},
      gpu_mem_write_bw_variation_bps_{
          frame_params.gpu_mem_write_bw_variation_bps.value_or(3051695995)},
      gpu_fan_speed_rpm_{
          frame_params.gpu_fan_speed_rpm.value_or(1070.2)},
      gpu_fan_speed_rpm_variation_rpm_{
          frame_params.gpu_fan_speed_rpm.value_or(100.2)},
      gpu_power_limited_percent_{
          frame_params.gpu_power_limited_percent.value_or(0.)},
      gpu_temp_limited_percent_{
          frame_params.gpu_temp_limited_percent.value_or(0.)},
      gpu_current_limited_percent_{
          frame_params.gpu_current_limited_percent.value_or(0.)},
      gpu_voltage_limited_percent_{
          frame_params.gpu_voltage_limited_percent.value_or(0.)},
      gpu_util_limited_percent_{
          frame_params.gpu_util_limited_percent.value_or(0.)},
      vram_power_limited_percent_{
          frame_params.vram_power_limited_percent.value_or(0.)},
      vram_temp_limited_percent_{
          frame_params.vram_temp_limited_percent.value_or(0.)},
      vram_current_limited_percent_{
          frame_params.vram_current_limited_percent.value_or(0.)},
      vram_voltage_limited_percent_{
          frame_params.vram_voltage_limited_percent.value_or(0.)},
      vram_util_limited_percent_{
          frame_params.vram_util_limited_percent.value_or(0.)},
      cpu_util_percent_{frame_params.cpu_util_percent.value_or(19.4)},
      cpu_util_variation_percent_{
          frame_params.cpu_util_variation_percent.value_or(5.86)},
      cpu_frequency_mhz_{frame_params.cpu_frequency_mhz.value_or(4212.9)},
      cpu_frequency_variation_mhz_{
          frame_params.cpu_frequency_variation_mhz.value_or(1070.2)} {

  QueryPerformanceFrequency(&qpc_frequency_);
  QueryPerformanceCounter(&start_qpc_);

  swap_chains_.resize(1);
  swap_chains_[0] = (uint64_t)1;
}

void PmFrameGenerator::GenerateFrames(int num_frames) {
  frames_.clear();
  frames_.resize(num_frames);
  pmft_frames_.clear();
  pmft_frames_.resize(num_frames);
  GeneratePresentData();
  GenerateGPUData();
  GenerateCPUData();
  return;
}

size_t PmFrameGenerator::GetNumFrames() {
  if (frames_.size() > 0) {
    return frames_.size() - 1;
  } else {
    return frames_.size();
  }
}

PmNsmFrameData PmFrameGenerator::GetFrameData(int frame_num) {
  PmNsmFrameData temp_frame{};
  if (frame_num >= 0 && frame_num < frames_.size()) {
    temp_frame = frames_[frame_num];
  }
  return temp_frame;
}

PM_FRAME_DATA PmFrameGenerator::GetPmFrameData(
    int frame_num, GpuTelemetryBitset gpu_telemetry_cap_bits,
    CpuTelemetryBitset cpu_telemetry_cap_bits) {
  PM_FRAME_DATA temp_frame{};
  if (frame_num >= 0 &&
      (frame_num < pmft_frames_.size() && (frame_num < frames_.size()))) {
    std::string temp_string = frames_[frame_num].present_event.application;
    if (temp_string.size() < sizeof(temp_frame.application)) {
      temp_string.copy(temp_frame.application, sizeof(temp_frame.application));
    }
    temp_frame.process_id = frames_[frame_num].present_event.ProcessId;
    temp_frame.swap_chain_address =
        frames_[frame_num].present_event.SwapChainAddress;
    temp_string = RuntimeToString(frames_[frame_num].present_event.Runtime);
    if (temp_string.size() < sizeof(temp_frame.runtime)) {
      temp_string.copy(temp_frame.runtime, sizeof(temp_frame.runtime));
    }
    temp_frame.sync_interval = frames_[frame_num].present_event.SyncInterval;
    temp_frame.present_flags = frames_[frame_num].present_event.PresentFlags;
    temp_frame.dropped = pmft_frames_[frame_num].dropped;
    temp_frame.time_in_seconds = pmft_frames_[frame_num].time_in_seconds;
    temp_frame.ms_in_present_api = pmft_frames_[frame_num].ms_in_present_api;
    temp_frame.ms_between_presents =
        pmft_frames_[frame_num].ms_between_presents;
    temp_frame.allows_tearing =
        frames_[frame_num].present_event.SupportsTearing;
    temp_frame.present_mode = pmft_frames_[frame_num].present_mode;
    temp_frame.ms_until_render_complete =
        pmft_frames_[frame_num].ms_until_render_complete;
    temp_frame.ms_until_displayed = pmft_frames_[frame_num].ms_until_displayed;
    temp_frame.ms_between_display_change =
        pmft_frames_[frame_num].ms_between_display_change;
    temp_frame.ms_until_render_start =
        pmft_frames_[frame_num].ms_until_render_start;
    temp_frame.qpc_time = pmft_frames_[frame_num].qpc_time;
    temp_frame.ms_since_input = pmft_frames_[frame_num].ms_until_input;
    temp_frame.ms_gpu_active = pmft_frames_[frame_num].ms_gpu_active;
    temp_frame.ms_gpu_video_active = pmft_frames_[frame_num].ms_gpu_video_active;

    // Copy power telemetry
    temp_frame.gpu_power_w.data = frames_[frame_num].power_telemetry.gpu_power_w;
    temp_frame.gpu_power_w.valid = gpu_telemetry_cap_bits[static_cast<size_t>(
        GpuTelemetryCapBits::gpu_power)];

    temp_frame.gpu_sustained_power_limit_w.data =
        frames_[frame_num].power_telemetry.gpu_sustained_power_limit_w;
    temp_frame.gpu_sustained_power_limit_w.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
        GpuTelemetryCapBits::gpu_sustained_power_limit)];

    temp_frame.gpu_voltage_v.data = frames_[frame_num].power_telemetry.gpu_voltage_v;
    temp_frame.gpu_voltage_v.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_voltage)];

    temp_frame.gpu_frequency_mhz.data =
        frames_[frame_num].power_telemetry.gpu_frequency_mhz;
    temp_frame.gpu_frequency_mhz.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
        GpuTelemetryCapBits::gpu_frequency)];

    temp_frame.gpu_temperature_c.data =
        frames_[frame_num].power_telemetry.gpu_temperature_c;
    temp_frame.gpu_temperature_c.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_temperature)];

    temp_frame.gpu_utilization.data =
        frames_[frame_num].power_telemetry.gpu_utilization;
    temp_frame.gpu_utilization.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_utilization)];

    temp_frame.gpu_render_compute_utilization.data =
        frames_[frame_num].power_telemetry.gpu_render_compute_utilization;
    temp_frame.gpu_render_compute_utilization.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_render_compute_utilization)];

    temp_frame.gpu_media_utilization.data =
        frames_[frame_num].power_telemetry.gpu_media_utilization;
    temp_frame.gpu_media_utilization.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_media_utilization)];

    temp_frame.vram_power_w.data =
        frames_[frame_num].power_telemetry.vram_power_w;
    temp_frame.vram_power_w.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_power)];

    temp_frame.vram_voltage_v.data =
        frames_[frame_num].power_telemetry.vram_voltage_v;
    temp_frame.vram_voltage_v.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
        GpuTelemetryCapBits::vram_voltage)];

    temp_frame.vram_frequency_mhz.data =
        frames_[frame_num].power_telemetry.vram_frequency_mhz;
    temp_frame.vram_frequency_mhz.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_frequency)];

    temp_frame.vram_effective_frequency_gbs.data =
        frames_[frame_num].power_telemetry.vram_effective_frequency_gbps;
    temp_frame.vram_effective_frequency_gbs.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_effective_frequency)];

    temp_frame.vram_temperature_c.data =
        frames_[frame_num].power_telemetry.vram_temperature_c;
    temp_frame.vram_temperature_c.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_temperature)];

    for (size_t i = 0; i < MAX_PM_FAN_COUNT; i++) {
      temp_frame.fan_speed_rpm[i].data =
          frames_[frame_num].power_telemetry.fan_speed_rpm[i];
      temp_frame.fan_speed_rpm[i].valid =
          gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::fan_speed_0) + i];
    }

    for (uint32_t i = 0; i < MAX_PM_PSU_COUNT; i++) {
      temp_frame.psu_type[i].data =
          TranslatePsuType(frames_[frame_num].power_telemetry.psu[i].psu_type);
      temp_frame.psu_type[i].valid = gpu_telemetry_cap_bits
          [static_cast<size_t>(GpuTelemetryCapBits::psu_info_0) + i];

      temp_frame.psu_power[i].data =
          frames_[frame_num].power_telemetry.psu[i].psu_power;
      temp_frame.psu_power[i].valid = gpu_telemetry_cap_bits
          [static_cast<size_t>(GpuTelemetryCapBits::psu_info_0) + i];

      temp_frame.psu_voltage[i].data =
          frames_[frame_num].power_telemetry.psu[i].psu_voltage;
      temp_frame.psu_voltage[i].valid = gpu_telemetry_cap_bits
          [static_cast<size_t>(GpuTelemetryCapBits::psu_info_0) + i];
    }

    temp_frame.gpu_mem_total_size_b.data =
        frames_[frame_num].power_telemetry.gpu_mem_total_size_b;
    temp_frame.gpu_mem_total_size_b.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_mem_size)];

    temp_frame.gpu_mem_used_b.data =
        frames_[frame_num].power_telemetry.gpu_mem_used_b;
    temp_frame.gpu_mem_used_b.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_mem_used)];

    temp_frame.gpu_mem_max_bandwidth_bps.data =
        frames_[frame_num].power_telemetry.gpu_mem_max_bandwidth_bps;
    temp_frame.gpu_mem_max_bandwidth_bps.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_mem_max_bandwidth)];

    temp_frame.gpu_mem_read_bandwidth_bps.data =
        frames_[frame_num].power_telemetry.gpu_mem_read_bandwidth_bps;
    temp_frame.gpu_mem_read_bandwidth_bps.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_mem_read_bandwidth)];

    temp_frame.gpu_mem_write_bandwidth_bps.data =
        frames_[frame_num].power_telemetry.gpu_mem_write_bandwidth_bps;
    temp_frame.gpu_mem_write_bandwidth_bps.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_mem_write_bandwidth)];

    temp_frame.gpu_power_limited.data =
        frames_[frame_num].power_telemetry.gpu_power_limited;
    temp_frame.gpu_power_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_power_limited)];

    temp_frame.gpu_temperature_limited.data =
        frames_[frame_num].power_telemetry.gpu_temperature_limited;
    temp_frame.gpu_temperature_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_temperature_limited)];

    temp_frame.gpu_current_limited.data =
        frames_[frame_num].power_telemetry.gpu_current_limited;
    temp_frame.gpu_current_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_current_limited)];

    temp_frame.gpu_voltage_limited.data =
        frames_[frame_num].power_telemetry.gpu_voltage_limited;
    temp_frame.gpu_voltage_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_voltage_limited)];

    temp_frame.gpu_utilization_limited.data =
        frames_[frame_num].power_telemetry.gpu_utilization_limited;
    temp_frame.gpu_utilization_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::gpu_utilization_limited)];

    temp_frame.vram_power_limited.data =
        frames_[frame_num].power_telemetry.vram_power_limited;
    temp_frame.vram_power_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_power_limited)];

    temp_frame.vram_temperature_limited.data =
        frames_[frame_num].power_telemetry.vram_temperature_limited;
    temp_frame.vram_temperature_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_temperature_limited)];

    temp_frame.vram_current_limited.data =
        frames_[frame_num].power_telemetry.vram_current_limited;
    temp_frame.vram_current_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_current_limited)];

    temp_frame.vram_voltage_limited.data =
        frames_[frame_num].power_telemetry.vram_voltage_limited;
    temp_frame.vram_voltage_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_voltage_limited)];

    temp_frame.vram_utilization_limited.data =
        frames_[frame_num].power_telemetry.vram_utilization_limited;
    temp_frame.vram_utilization_limited.valid =
        gpu_telemetry_cap_bits[static_cast<size_t>(
            GpuTelemetryCapBits::vram_utilization_limited)];

    // Cpu telemetry
    temp_frame.cpu_utilization.data =
        frames_[frame_num].cpu_telemetry.cpu_utilization;
    temp_frame.cpu_utilization.valid =
        cpu_telemetry_cap_bits[static_cast<size_t>(
            CpuTelemetryCapBits::cpu_utilization)];

    temp_frame.cpu_frequency.data =
        frames_[frame_num].cpu_telemetry.cpu_frequency;
    temp_frame.cpu_frequency.valid =
        cpu_telemetry_cap_bits[static_cast<size_t>(
            CpuTelemetryCapBits::cpu_frequency)];
  }
  return temp_frame;
}

void PmFrameGenerator::GeneratePresentData() {
  if (frames_.size() != pmft_frames_.size()) {
    return;
  }

  int swap_chain_idx = 0;
  uint64_t last_displayed_screen_time = 0;
  for (int i = 0; i < (int)frames_.size(); i++) {
    if (app_name_.size() < sizeof(frames_[i].present_event.application)) {
      app_name_.copy(frames_[i].present_event.application,
                     sizeof(frames_[i].present_event.application));
    }
    frames_[i].present_event.ProcessId = process_id_;
    frames_[i].present_event.SwapChainAddress = swap_chains_[swap_chain_idx];
    pmft_frames_[i].swap_chain = swap_chains_[swap_chain_idx];
    // Set the next swap chain index
    swap_chain_idx += 1;
    swap_chain_idx = swap_chain_idx % swap_chains_.size();
    frames_[i].present_event.Runtime = runtime_;
    frames_[i].present_event.SyncInterval = sync_interval_;
    frames_[i].present_event.PresentFlags = present_flags_;
    // Using the specified percent dropped set the final state of the
    // present
    if (uniform_random_gen_.Generate(0., 100.) < percent_dropped_) {
      frames_[i].present_event.FinalState = PresentResult::Discarded;
    } else {
      frames_[i].present_event.FinalState = PresentResult::Presented;
    }

    frames_[i].present_event.PresentMode = present_mode_;
    pmft_frames_[i].present_mode = TranslatePresentMode(present_mode_);
    // Calculate the ms between presents
    pmft_frames_[i].ms_between_presents = GetAlteredTimingValue(
        between_presents_ms_, between_presents_variation_ms_);
    // Convert ms between presents to qpc ticks
    auto qpc_ticks_between_presents = SecondsDeltaToQpc(
        pmft_frames_[i].ms_between_presents / 1000., qpc_frequency_);
    // Using the ms between presents calculate the frame qpc times.
    if (i == 0) {
      // Set the first frame to the QPC of when the frame generator was
      // initialized.
      frames_[i].present_event.PresentStartTime = start_qpc_.QuadPart;
      // Also on the first frame calculate a qpc for the last presented qpc
      // using the created ms_between_presents and the start qpc.
      frames_[i].present_event.last_present_qpc =
          frames_[i].present_event.PresentStartTime -
          qpc_ticks_between_presents;

    } else {
      frames_[i].present_event.PresentStartTime =
          frames_[i - 1].present_event.PresentStartTime +
          qpc_ticks_between_presents;
      frames_[i].present_event.last_present_qpc =
          frames_[i - 1].present_event.PresentStartTime;
    }
    pmft_frames_[i].qpc_time = frames_[i].present_event.PresentStartTime;
    pmft_frames_[i].time_in_seconds = QpcDeltaToSeconds(
        pmft_frames_[i].qpc_time - pmft_frames_[0].qpc_time, qpc_frequency_);

    pmft_frames_[i].ms_in_present_api =
        GetAlteredTimingValue(
        in_present_api_ms_, in_present_api_variation_ms_);
    frames_[i].present_event.TimeInPresent =
        SecondsDeltaToQpc(pmft_frames_[i].ms_in_present_api / 1000.,
                          qpc_frequency_);
    pmft_frames_[i].ms_until_render_start = GetAlteredTimingValue(
        until_render_start_ms_, until_render_start_variation_ms_);
    if (pmft_frames_[i].ms_until_render_start > 0.) {
      // If positive this means GPU start time occurred after present
      // start. Convert the render start time from ms to qpc ticks
      frames_[i].present_event.GPUStartTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_render_start / 1000., qpc_frequency_);
      // Add in the start time of the present to get the correct gpu
      // start time
      frames_[i].present_event.GPUStartTime =
          frames_[i].present_event.PresentStartTime +
          frames_[i].present_event.GPUStartTime;
    } else {
      // GPUStartTime is an unsigned int 64 so we need to convert to
      // positive.
      frames_[i].present_event.GPUStartTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_render_start / -1000., qpc_frequency_);
      // SUBTRACT the start time of the present to get the correct gpu
      // start time
      frames_[i].present_event.GPUStartTime =
          frames_[i].present_event.PresentStartTime -
          frames_[i].present_event.GPUStartTime;
    }

    pmft_frames_[i].ms_until_render_complete = GetAlteredTimingValue(
        until_render_complete_ms_, until_render_complete_variation_ms_);
    if (pmft_frames_[i].ms_until_render_complete > 0.0) {
      frames_[i].present_event.ReadyTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_render_complete / 1000., qpc_frequency_);
      frames_[i].present_event.ReadyTime =
          frames_[i].present_event.PresentStartTime +
                                           frames_[i].present_event.ReadyTime;
    } else {
      // ReadyTime is also an unsigned int 64, same as above.
      frames_[i].present_event.ReadyTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_render_complete / -1000., qpc_frequency_);
      frames_[i].present_event.ReadyTime =
          frames_[i].present_event.PresentStartTime -
                                           frames_[i].present_event.ReadyTime;
    }

    if (frames_[i].present_event.FinalState == PresentResult::Presented) {
      pmft_frames_[i].dropped = false;
      pmft_frames_[i].ms_until_displayed = GetAlteredTimingValue(
          until_displayed_ms_, until_displayed_variation_ms_);
      frames_[i].present_event.ScreenTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_displayed / 1000., qpc_frequency_);
      frames_[i].present_event.ScreenTime +=
          frames_[i].present_event.PresentStartTime;
      if (last_displayed_screen_time != 0) {
        pmft_frames_[i].ms_between_display_change = QpcDeltaToMs(
            frames_[i].present_event.ScreenTime - last_displayed_screen_time,
            qpc_frequency_);
        frames_[i].present_event.last_displayed_qpc =
            last_displayed_screen_time;

      } else {
        pmft_frames_[i].ms_between_display_change = 0.;
        frames_[i].present_event.last_displayed_qpc = 0;
      }
      last_displayed_screen_time = frames_[i].present_event.ScreenTime;
    } else {
      pmft_frames_[i].ms_until_displayed = 0;
      pmft_frames_[i].ms_between_display_change = 0.;
      pmft_frames_[i].dropped = true;
      frames_[i].present_event.ScreenTime = 0;
    }

    // Internal frame data fields; Remove for public build
    pmft_frames_[i].ms_until_input = GetAlteredTimingValue(since_input_ms_,
                                            since_input_variation_ms_);
    if (pmft_frames_[i].ms_until_input != 0.) {
      frames_[i].present_event.InputTime = SecondsDeltaToQpc(
          pmft_frames_[i].ms_until_input / 1000., qpc_frequency_);
      frames_[i].present_event.InputTime =
          frames_[i].present_event.PresentStartTime -
          frames_[i].present_event.InputTime;
    } else {
      frames_[i].present_event.InputTime = 0;
    }
    pmft_frames_[i].ms_gpu_active =
        GetAlteredTimingValue(gpu_active_ms_, gpu_active_variation_ms_);
    frames_[i].present_event.GPUDuration = SecondsDeltaToQpc(
        pmft_frames_[i].ms_gpu_active / 1000., qpc_frequency_);
    pmft_frames_[i].ms_gpu_video_active =
        GetAlteredTimingValue(gpu_video_active_ms_, gpu_video_active_variation_ms_);
    frames_[i].present_event.GPUVideoDuration = SecondsDeltaToQpc(
        pmft_frames_[i].ms_gpu_video_active / 1000., qpc_frequency_);
  }
}

void PmFrameGenerator::GenerateGPUData() {
  for (int i = 0; i < (int)frames_.size(); i++) {
    frames_[i].power_telemetry.gpu_power_w =
        GetAlteredTimingValue(gpu_power_w_, gpu_power_variation_w_);
    frames_[i].power_telemetry.gpu_sustained_power_limit_w =
        GetAlteredTimingValue(gpu_sustained_power_limit_w_,
                                  gpu_sustained_power_limit_variation_w_);
    frames_[i].power_telemetry.gpu_voltage_v =
        GetAlteredTimingValue(gpu_voltage_v_, gpu_voltage_variation_v_);
    frames_[i].power_telemetry.gpu_frequency_mhz =
        GetAlteredTimingValue(
        gpu_frequency_mhz_, gpu_frequency_variation_mhz_);
    frames_[i].power_telemetry.gpu_temperature_c =
        GetAlteredTimingValue(gpu_temp_c_, gpu_temp_variation_c_);
    frames_[i].power_telemetry.gpu_utilization = GetAlteredTimingValue(
        gpu_util_percent_, gpu_util_variation_percent_);
    frames_[i].power_telemetry.gpu_render_compute_utilization =
        GetAlteredTimingValue(gpu_render_compute_util_percent_,
                                  gpu_render_compute_util_variation_percent_);
    frames_[i].power_telemetry.gpu_media_utilization =
        GetAlteredTimingValue(gpu_media_util_percent_,
                                  gpu_media_util_variation_percent_);
    frames_[i].power_telemetry.vram_power_w =
        GetAlteredTimingValue(vram_power_w_, vram_power_variation_w_);
    frames_[i].power_telemetry.vram_voltage_v =
        GetAlteredTimingValue(vram_voltage_v_, vram_voltage_variation_v_);
    frames_[i].power_telemetry.vram_frequency_mhz = GetAlteredTimingValue(
        vram_frequency_mhz_, vram_frequency_variation_mhz_);
    frames_[i].power_telemetry.vram_effective_frequency_gbps =
        GetAlteredTimingValue(vram_effective_frequency_gbps_,
                                  vram_effective_frequency_variation_gbps_);
    frames_[i].power_telemetry.vram_temperature_c =
        GetAlteredTimingValue(vram_temp_c_, vram_temp_variation_c_);
    frames_[i].power_telemetry.gpu_mem_total_size_b = GetAlteredTimingValue(
        gpu_mem_total_size_b_, gpu_mem_total_size_variation_b_);
    frames_[i].power_telemetry.gpu_mem_used_b =
        GetAlteredTimingValue(gpu_mem_used_b_, gpu_mem_used_variation_b_);
    frames_[i].power_telemetry.gpu_mem_max_bandwidth_bps =
        GetAlteredTimingValue(gpu_mem_max_bw_gbps_,
                                  gpu_mem_max_bw_variation_gbps_);
    frames_[i].power_telemetry.gpu_mem_read_bandwidth_bps =
        (double)GetAlteredTimingValue(gpu_mem_read_bw_bps_,
                                          gpu_mem_read_bw_variation_bps_);
    frames_[i].power_telemetry.gpu_mem_write_bandwidth_bps =
        (double)GetAlteredTimingValue(gpu_mem_write_bw_bps_,
                                          gpu_mem_write_bw_variation_bps_);
    frames_[i].power_telemetry.fan_speed_rpm[0] = GetAlteredTimingValue(
          gpu_fan_speed_rpm_, gpu_fan_speed_rpm_variation_rpm_);
    frames_[i].power_telemetry.gpu_power_limited =
        IsLimited(gpu_power_limited_percent_);
    frames_[i].power_telemetry.gpu_temperature_limited =
        IsLimited(gpu_util_limited_percent_);
    frames_[i].power_telemetry.gpu_current_limited =
        IsLimited(gpu_current_limited_percent_);
    frames_[i].power_telemetry.gpu_voltage_limited =
        IsLimited(gpu_voltage_limited_percent_);
    frames_[i].power_telemetry.gpu_utilization_limited =
        IsLimited(gpu_util_limited_percent_);
    frames_[i].power_telemetry.vram_power_limited =
        IsLimited(vram_power_limited_percent_);
    frames_[i].power_telemetry.vram_temperature_limited =
        IsLimited(vram_util_limited_percent_);
    frames_[i].power_telemetry.vram_current_limited =
        IsLimited(vram_current_limited_percent_);
    frames_[i].power_telemetry.vram_voltage_limited =
        IsLimited(vram_voltage_limited_percent_);
    frames_[i].power_telemetry.vram_utilization_limited =
        IsLimited(vram_util_limited_percent_);
  }
}

void PmFrameGenerator::GenerateCPUData() {
  for (int i = 0; i < (int)frames_.size(); i++) {
    frames_[i].cpu_telemetry.cpu_utilization = GetAlteredTimingValue(
        cpu_util_percent_, cpu_util_variation_percent_);
    frames_[i].cpu_telemetry.cpu_frequency = GetAlteredTimingValue(
        cpu_frequency_mhz_, cpu_frequency_variation_mhz_);
  }
}

void PmFrameGenerator::CalcMetricStats(std::vector<double>& data,
                                       PM_METRIC_DOUBLE_DATA& metric,
                                       bool valid) {
  if (data.size() > 1) {
    // Before sorting, grab the raw data.
    size_t middle_index = data.size() / 2;
    metric.raw = data[middle_index];
    std::sort(data.begin(), data.end());
    metric.low = data[0];
    metric.high = data[data.size() - 1];

    int window_size = (int)data.size();
    auto sum = std::accumulate(data.begin(), data.end(), 0.);
    if (sum != 0) {
      metric.avg = sum / window_size;
    }

    metric.percentile_90 = GetPercentile(data, 0.1);
    metric.percentile_95 = GetPercentile(data, 0.05);
    metric.percentile_99 = GetPercentile(data, 0.01);
  } else if (data.size() == 1) {
    metric.low = data[0];
    metric.high = data[0];
    metric.avg = data[0];
    metric.percentile_90 = data[0];
    metric.percentile_95 = data[0];
    metric.percentile_99 = data[0];
  } else {
    metric.low = 0.;
    metric.high = 0.;
    metric.avg = 0.;
    metric.percentile_90 = 0.;
    metric.percentile_95 = 0.;
    metric.percentile_99 = 0.;
  }
  metric.valid = valid;
  return;
}

// Calculate percentile using linear interpolation between the closet ranks
// method
double PmFrameGenerator::GetPercentile(std::vector<double>& data,
                                       double percentile) {
  double integral_part_as_double;
  double fractpart =
      modf(((percentile * (static_cast<double>(data.size() - 1))) + 1),
           &integral_part_as_double);

  // Subtract off one from the integral_part as we are zero based and the
  // calculation above is based one based
  uint32_t integral_part = static_cast<uint32_t>(integral_part_as_double) - 1;
  uint32_t next_idx = integral_part + 1;
  // Before we access the vector data ensure that our calculated index values
  // are not out of range
  if (integral_part < data.size() || next_idx < data.size()) {
    return data[integral_part] +
           (fractpart * (data[next_idx] - data[integral_part]));
  } else {
    return 0.0f;
  }
}

bool PmFrameGenerator::CalculateFpsMetrics(
    uint32_t start_frame, double window_size_in_ms,
    std::vector<PM_FPS_DATA>& fps_metrics) {
  std::unordered_map<uint64_t, fps_swap_chain_data> swap_chain_data;
  if (start_frame > pmft_frames_.size()) {
    return false;
  }

  // Get the qpc for the start frame
  uint64_t start_frame_qpc = pmft_frames_[start_frame].qpc_time;

  // Calculate number of ticks based on the passed in window size
  uint64_t window_size_in_ticks =
      SecondsDeltaToQpc(window_size_in_ms / 1000., qpc_frequency_);
  uint64_t calculated_end_frame_qpc = start_frame_qpc - window_size_in_ticks;

  for (uint32_t current_frame_number = start_frame; current_frame_number > 0;
       current_frame_number--) {
    auto result = swap_chain_data.emplace(
        pmft_frames_[current_frame_number].swap_chain, fps_swap_chain_data());
    auto swap_chain = &result.first->second;
    if (result.second) {
      swap_chain->num_presents = 1;
      // TODO Need to define sync interval in pmft structure
      // swap_chain->sync_interval
      swap_chain->present_mode =
          pmft_frames_[current_frame_number].present_mode;
      swap_chain->gpu_sum_ms.push_back(
          pmft_frames_[current_frame_number].ms_gpu_active);
      swap_chain->time_in_s = 0.;
      swap_chain->cpu_n_time =
          pmft_frames_[current_frame_number].time_in_seconds;
      swap_chain->cpu_0_time = swap_chain->cpu_n_time;
      if (pmft_frames_[current_frame_number].dropped == false) {
        swap_chain->mLastDisplayedScreenTime =
            pmft_frames_[current_frame_number].time_in_seconds +
            (pmft_frames_[current_frame_number].ms_until_displayed / 1000.);
        swap_chain->display_0_screen_time = swap_chain->mLastDisplayedScreenTime;
        swap_chain->dropped.push_back(0);
      } else {
        swap_chain->dropped.push_back(1);
      }
    } else {
      if (pmft_frames_[current_frame_number].qpc_time >
          calculated_end_frame_qpc) {
        swap_chain->num_presents++;
        swap_chain->present_mode =
            pmft_frames_[current_frame_number].present_mode;
        swap_chain->gpu_sum_ms.push_back(
            pmft_frames_[current_frame_number].ms_gpu_active);
        swap_chain->time_in_s =
            swap_chain->cpu_0_time -
            pmft_frames_[current_frame_number].time_in_seconds;
        swap_chain->cpu_0_time =
            pmft_frames_[current_frame_number].time_in_seconds;
        if (swap_chain->time_in_s != 0.) {
          // Convert to ms and store frametime
          swap_chain->frame_times_ms.push_back(swap_chain->time_in_s * 1000.);
          swap_chain->presented_fps.push_back(1. / swap_chain->time_in_s);
        }
        if (pmft_frames_[current_frame_number].dropped == false) {
          auto current_display_screen_time_s =
              pmft_frames_[current_frame_number].time_in_seconds +
              (pmft_frames_[current_frame_number].ms_until_displayed / 1000.);
          if (swap_chain->display_0_screen_time != 0.) {
            swap_chain->display_fps.push_back(
                1. / (swap_chain->display_0_screen_time -
                      current_display_screen_time_s));
          } else {
            swap_chain->mLastDisplayedScreenTime = current_display_screen_time_s;
          }
          swap_chain->display_0_screen_time = current_display_screen_time_s;
          swap_chain->dropped.push_back(0);
        } else {
          swap_chain->dropped.push_back(1);
        }
      } else {
        break;
      }
    }
  }

  fps_metrics.clear();
  PM_FPS_DATA temp_fps_data{};
  for (auto pair : swap_chain_data) {
    temp_fps_data.swap_chain = pair.first;
    auto swap_chain = pair.second;
    CalcMetricStats(swap_chain.display_fps, temp_fps_data.displayed_fps);
    CalcMetricStats(swap_chain.presented_fps, temp_fps_data.presented_fps);
    CalcMetricStats(swap_chain.frame_times_ms, temp_fps_data.frame_time_ms);
    CalcMetricStats(swap_chain.gpu_sum_ms, temp_fps_data.gpu_busy);
    // Overwrite the average both the display and cpu average fps.
    auto avg_fps =
        swap_chain.mLastDisplayedScreenTime - swap_chain.display_0_screen_time;
    avg_fps /= swap_chain.display_fps.size();
    avg_fps = 1. / avg_fps;
    temp_fps_data.displayed_fps.avg = avg_fps;
    avg_fps = swap_chain.cpu_n_time - swap_chain.cpu_0_time;
    avg_fps /= swap_chain.presented_fps.size();
    avg_fps = 1. / avg_fps;
    temp_fps_data.presented_fps.avg = avg_fps;
    temp_fps_data.present_mode = swap_chain.present_mode;
    temp_fps_data.num_presents = swap_chain.num_presents;
    temp_fps_data.sync_interval = swap_chain.sync_interval;
    CalcMetricStats(swap_chain.dropped, temp_fps_data.percent_dropped_frames);
    temp_fps_data.percent_dropped_frames.avg *= 100.;

    fps_metrics.emplace_back(temp_fps_data);
    temp_fps_data = {};
  }
  return true;
}

void PmFrameGenerator::CalculateLatencyMetrics(
    uint32_t start_frame, double window_size_in_ms,
    std::vector<PM_GFX_LATENCY_DATA>& latency_metrics) {

  std::unordered_map<uint64_t, latency_swap_chain_data> swap_chain_data;

  if (start_frame > pmft_frames_.size()) {
    return;
  }

  // Get the qpc for the start frame
  uint64_t start_frame_qpc = pmft_frames_[start_frame].qpc_time;

  // Calculate number of ticks based on the passed in window size
  uint64_t window_size_in_ticks =
      SecondsDeltaToQpc(window_size_in_ms / 1000., qpc_frequency_);
  uint64_t calculated_end_frame_qpc = start_frame_qpc - window_size_in_ticks;

  for (uint32_t current_frame_number = start_frame; current_frame_number > 0;
       current_frame_number--) {
    auto result =
        swap_chain_data.emplace(pmft_frames_[current_frame_number].swap_chain,
                                latency_swap_chain_data());
    auto swap_chain = &result.first->second;
    if (result.second) {
      swap_chain->render_latency_ms.clear();
      swap_chain->display_latency_ms.clear();
    } else {
      if (pmft_frames_[current_frame_number].qpc_time >
          calculated_end_frame_qpc) {
        if (pmft_frames_[current_frame_number].ms_between_display_change !=
            0.) {
          swap_chain->render_latency_ms.push_back(
              pmft_frames_[current_frame_number].ms_until_displayed);
          swap_chain->display_latency_ms.push_back(
              pmft_frames_[current_frame_number].ms_until_displayed -
              pmft_frames_[current_frame_number].ms_until_render_complete);
        }
      } else {
        break;
      }
    }
  }

  latency_metrics.clear();
  PM_GFX_LATENCY_DATA temp_latency_data{};
  for (auto pair : swap_chain_data) {
    temp_latency_data.swap_chain = pair.first;
    auto swap_chain = pair.second;
    CalcMetricStats(swap_chain.render_latency_ms,
                    temp_latency_data.render_latency_ms);
    CalcMetricStats(swap_chain.display_latency_ms,
                    temp_latency_data.display_latency_ms);
    latency_metrics.emplace_back(temp_latency_data);
    temp_latency_data = {};
  }
  return;
}

void PmFrameGenerator::CalculateGpuMetrics(
    uint32_t start_frame, double window_size_in_ms,
    GpuTelemetryBitset gpu_telemetry_cap_bits, PM_GPU_DATA& gpu_metrics) {
  if (start_frame > pmft_frames_.size()) {
    return;
  }

  // Get the qpc for the start frame
  uint64_t start_frame_qpc = pmft_frames_[start_frame].qpc_time;

  bool gpu_mem_util_enabled = (gpu_telemetry_cap_bits[static_cast<size_t>(
                                   GpuTelemetryCapBits::gpu_mem_size)] &&
                               gpu_telemetry_cap_bits[static_cast<size_t>(
                                   GpuTelemetryCapBits::gpu_mem_used)]);

  // Calculate number of ticks based on the passed in window size
  uint64_t window_size_in_ticks =
      SecondsDeltaToQpc(window_size_in_ms / 1000., qpc_frequency_);
  uint64_t calculated_end_frame_qpc = start_frame_qpc - window_size_in_ticks;
  gpu_data calculated_gpu_metrics{};
  for (uint32_t current_frame_number = start_frame; current_frame_number > 0;
       current_frame_number--) {
    if (pmft_frames_[current_frame_number].qpc_time >
        calculated_end_frame_qpc) {
      if (gpu_telemetry_cap_bits[static_cast<size_t>(GpuTelemetryCapBits::gpu_power)]) {
        calculated_gpu_metrics.gpu_power_w.push_back(
            frames_[current_frame_number].power_telemetry.gpu_power_w);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_sustained_power_limit)]) {
        calculated_gpu_metrics.gpu_sustained_power_limit_w.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_sustained_power_limit_w);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_voltage)]) {
        calculated_gpu_metrics.gpu_voltage_v.push_back(
            frames_[current_frame_number].power_telemetry.gpu_voltage_v);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_frequency)]) {
        calculated_gpu_metrics.gpu_frequency_mhz.push_back(
            frames_[current_frame_number].power_telemetry.gpu_frequency_mhz);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_temperature)]) {
        calculated_gpu_metrics.gpu_temp_c.push_back(
            frames_[current_frame_number].power_telemetry.gpu_temperature_c);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_utilization)]) {
        calculated_gpu_metrics.gpu_util_percent.push_back(
            frames_[current_frame_number].power_telemetry.gpu_utilization);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_render_compute_utilization)]) {
        calculated_gpu_metrics.gpu_render_compute_util_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_render_compute_utilization);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_media_utilization)]) {
        calculated_gpu_metrics.gpu_media_util_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_media_utilization);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_power)]) {
        calculated_gpu_metrics.vram_power_w.push_back(
            frames_[current_frame_number].power_telemetry.vram_power_w);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_voltage)]) {
        calculated_gpu_metrics.vram_voltage_v.push_back(
            frames_[current_frame_number].power_telemetry.vram_voltage_v);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_frequency)]) {
        calculated_gpu_metrics.vram_frequency_mhz.push_back(
            frames_[current_frame_number].power_telemetry.vram_frequency_mhz);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_effective_frequency)]) {
        calculated_gpu_metrics.vram_effective_frequency_gbps.push_back(
            frames_[current_frame_number]
                .power_telemetry.vram_effective_frequency_gbps);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_temperature)]) {
        calculated_gpu_metrics.vram_temp_c.push_back(
            frames_[current_frame_number].power_telemetry.vram_temperature_c);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_mem_size)]) {
        calculated_gpu_metrics.gpu_mem_total_size_b.push_back(
            (double)frames_[current_frame_number]
                .power_telemetry.gpu_mem_total_size_b);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_mem_used)]) {
        calculated_gpu_metrics.gpu_mem_used_b.push_back(
            (double)frames_[current_frame_number]
                .power_telemetry.gpu_mem_used_b);
      }
      // gpu mem utilization is calculated from the total gpu memory
      // and the used gpu memory
      if (gpu_mem_util_enabled) {
        if (frames_[current_frame_number]
                .power_telemetry.gpu_mem_total_size_b != 0.) {
          calculated_gpu_metrics.gpu_mem_util_percent.push_back(
              100. *
              double(frames_[current_frame_number]
                         .power_telemetry.gpu_mem_used_b) /
              frames_[current_frame_number]
                  .power_telemetry.gpu_mem_total_size_b);
        } else {
          calculated_gpu_metrics.gpu_mem_util_percent.push_back(0.);
        }
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_mem_max_bandwidth)]) {
        calculated_gpu_metrics.gpu_mem_max_bw_gbps.push_back(
            (double)frames_[current_frame_number]
                .power_telemetry.gpu_mem_max_bandwidth_bps);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_mem_read_bandwidth)]) {
        calculated_gpu_metrics.gpu_mem_read_bw_bps.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_mem_read_bandwidth_bps);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_mem_write_bandwidth)]) {
        calculated_gpu_metrics.gpu_mem_write_bw_bps.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_mem_write_bandwidth_bps);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::fan_speed_0)]) {
        calculated_gpu_metrics.gpu_fan_speed_rpm.push_back(
            frames_[current_frame_number].power_telemetry.fan_speed_rpm[0]);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_power_limited)]) {
        calculated_gpu_metrics.gpu_power_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.gpu_power_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_temperature_limited)]) {
        calculated_gpu_metrics.gpu_temp_limited_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_temperature_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_current_limited)]) {
        calculated_gpu_metrics.gpu_current_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.gpu_current_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_voltage_limited)]) {
        calculated_gpu_metrics.gpu_voltage_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.gpu_voltage_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::gpu_utilization_limited)]) {
        calculated_gpu_metrics.gpu_util_limited_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.gpu_utilization_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_power_limited)]) {
        calculated_gpu_metrics.vram_power_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.vram_power_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_temperature_limited)]) {
        calculated_gpu_metrics.vram_temp_limited_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.vram_temperature_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_current_limited)]) {
        calculated_gpu_metrics.vram_current_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.vram_current_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_voltage_limited)]) {
        calculated_gpu_metrics.vram_voltage_limited_percent.push_back(
            frames_[current_frame_number].power_telemetry.vram_voltage_limited);
      }
      if (gpu_telemetry_cap_bits[static_cast<size_t>(
              GpuTelemetryCapBits::vram_utilization_limited)]) {
        calculated_gpu_metrics.vram_util_limited_percent.push_back(
            frames_[current_frame_number]
                .power_telemetry.vram_utilization_limited);
      }

    } else {
      break;
    }
  }

  CalcMetricStats(calculated_gpu_metrics.gpu_power_w, gpu_metrics.gpu_power_w,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_power)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_sustained_power_limit_w,
                  gpu_metrics.gpu_sustained_power_limit_w,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_sustained_power_limit)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_voltage_v,
                  gpu_metrics.gpu_voltage_v,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_voltage)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_frequency_mhz,
                  gpu_metrics.gpu_frequency_mhz,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_frequency)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_temp_c,
                  gpu_metrics.gpu_temperature_c,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_temperature)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_fan_speed_rpm,
                  gpu_metrics.gpu_fan_speed_rpm[0],
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::fan_speed_0)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_util_percent,
                  gpu_metrics.gpu_utilization,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_utilization)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_render_compute_util_percent,
                  gpu_metrics.gpu_render_compute_utilization,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_render_compute_utilization)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_media_util_percent,
                  gpu_metrics.gpu_media_utilization,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_media_utilization)]);
  CalcMetricStats(calculated_gpu_metrics.vram_power_w, gpu_metrics.vram_power_w,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_power)]);
  CalcMetricStats(calculated_gpu_metrics.vram_voltage_v,
                  gpu_metrics.vram_voltage_v,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_voltage)]);
  CalcMetricStats(calculated_gpu_metrics.vram_frequency_mhz,
                  gpu_metrics.vram_frequency_mhz,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_frequency)]);
  CalcMetricStats(calculated_gpu_metrics.vram_effective_frequency_gbps,
                  gpu_metrics.vram_effective_frequency_gbps,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_effective_frequency)]);
  CalcMetricStats(calculated_gpu_metrics.vram_temp_c,
                  gpu_metrics.vram_temperature_c,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_temperature)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_total_size_b,
                  gpu_metrics.gpu_mem_total_size_b,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_mem_size)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_used_b,
                  gpu_metrics.gpu_mem_used_b,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_mem_used)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_util_percent,
                  gpu_metrics.gpu_mem_utilization, gpu_mem_util_enabled);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_max_bw_gbps,
                  gpu_metrics.gpu_mem_max_bandwidth_bps,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_mem_max_bandwidth)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_read_bw_bps,
                  gpu_metrics.gpu_mem_read_bandwidth_bps,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_mem_max_bandwidth)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_mem_write_bw_bps,
                  gpu_metrics.gpu_mem_write_bandwidth_bps,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_mem_write_bandwidth)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_power_limited_percent,
                  gpu_metrics.gpu_power_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_power_limited)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_temp_limited_percent,
                  gpu_metrics.gpu_temperature_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_temperature_limited)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_current_limited_percent,
                  gpu_metrics.gpu_current_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_current_limited)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_voltage_limited_percent,
                  gpu_metrics.gpu_voltage_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_voltage_limited)]);
  CalcMetricStats(calculated_gpu_metrics.gpu_util_limited_percent,
                  gpu_metrics.gpu_utilization_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::gpu_utilization_limited)]);
  CalcMetricStats(calculated_gpu_metrics.vram_power_limited_percent,
                  gpu_metrics.vram_power_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_power_limited)]);
  CalcMetricStats(calculated_gpu_metrics.vram_temp_limited_percent,
                  gpu_metrics.vram_temperature_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_temperature_limited)]);
  CalcMetricStats(calculated_gpu_metrics.vram_current_limited_percent,
                  gpu_metrics.vram_current_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_current_limited)]);
  CalcMetricStats(calculated_gpu_metrics.vram_voltage_limited_percent,
                  gpu_metrics.vram_voltage_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_voltage_limited)]);
  CalcMetricStats(calculated_gpu_metrics.vram_util_limited_percent,
                  gpu_metrics.vram_utilization_limited,
                  gpu_telemetry_cap_bits[static_cast<size_t>(
                      GpuTelemetryCapBits::vram_utilization_limited)]);
}

void PmFrameGenerator::CalculateCpuMetrics(
    uint32_t start_frame, double window_size_in_ms,
    CpuTelemetryBitset cpu_telemetry_cap_bits,
    PM_CPU_DATA& cpu_metrics) {
  if (start_frame > pmft_frames_.size()) {
    return;
  }

  // Get the qpc for the start frame
  uint64_t start_frame_qpc = pmft_frames_[start_frame].qpc_time;

  // Calculate number of ticks based on the passed in window size
  uint64_t window_size_in_ticks =
      SecondsDeltaToQpc(window_size_in_ms / 1000., qpc_frequency_);
  uint64_t calculated_end_frame_qpc = start_frame_qpc - window_size_in_ticks;
  cpu_data calculated_cpu_metrics{};
  for (uint32_t current_frame_number = start_frame; current_frame_number > 0;
       current_frame_number--) {
    if (pmft_frames_[current_frame_number].qpc_time >
        calculated_end_frame_qpc) {
      if (cpu_telemetry_cap_bits[static_cast<size_t>(
              CpuTelemetryCapBits::cpu_utilization)]) {
        calculated_cpu_metrics.cpu_util_percent.push_back(
            frames_[current_frame_number].cpu_telemetry.cpu_utilization);
      }
      if (cpu_telemetry_cap_bits[static_cast<size_t>(
              CpuTelemetryCapBits::cpu_frequency)]) {
        calculated_cpu_metrics.cpu_frequency_mhz.push_back(
            frames_[current_frame_number].cpu_telemetry.cpu_frequency);
      }
    }
  }

  CalcMetricStats(calculated_cpu_metrics.cpu_util_percent,
                  cpu_metrics.cpu_utilization,
                  cpu_telemetry_cap_bits[static_cast<size_t>(
                      CpuTelemetryCapBits::cpu_utilization)]);
  CalcMetricStats(calculated_cpu_metrics.cpu_frequency_mhz,
                  cpu_metrics.cpu_frequency,
                  cpu_telemetry_cap_bits[static_cast<size_t>(
                      CpuTelemetryCapBits::cpu_frequency)]);
}