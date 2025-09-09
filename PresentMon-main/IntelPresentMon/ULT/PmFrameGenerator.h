#pragma once

#include <numeric>
#include <random>

#include "../PresentMonUtils/MemBuffer.h"
#include "../PresentMonUtils/PresentDataUtils.h"
#include "../PresentMonUtils/PresentMonNamedPipe.h"
#include "../PresentMonUtils/QPCUtils.h"

struct PMFrameTimingInformation {
  uint64_t swap_chain = 0;
  bool dropped = false;
  double time_in_seconds = 0.;
  double ms_between_presents = 0.;
  double ms_in_present_api = 0.;
  double ms_until_render_start = 0.;
  double ms_until_render_complete = 0.;
  double ms_until_displayed = 0.;
  double ms_between_display_change = 0.;
  uint64_t qpc_time = 0;
  PM_PRESENT_MODE present_mode =
      PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_INDEPENDENT_FLIP;
  double ms_until_input = 0.;
  double ms_gpu_active = 0.;
  double ms_gpu_video_active = 0.;
};

template <typename T>
concept Numeric = (std::is_integral_v<T> ||
                   std::is_floating_point_v<T>)&&!std::same_as<T, bool>;

class PmFrameGenerator {
 public:
  struct FrameParams {
    std::optional<std::string> app_name;
    std::optional<uint32_t> process_id;
    std::optional<Runtime> runtime;
    std::optional<int32_t> sync_interval;
    std::optional<uint32_t> present_flags;
    std::optional<double> percent_dropped;
    std::optional<double> in_present_api_ms;
    std::optional<double> in_present_api_variation_ms;
    std::optional<double> between_presents_ms;
    std::optional<double> between_presents_variation_ms;
    std::optional<double> percent_tearing;
    std::optional<PresentMode> present_mode;
    std::optional<double> until_render_complete_ms;
    std::optional<double> until_render_complete_variation_ms;
    std::optional<double> until_displayed_ms;
    std::optional<double> until_displayed_variation_ms;
    std::optional<double> until_display_change_ms;
    std::optional<double> until_display_change_variation_ms;
    std::optional<double> until_render_start_ms;
    std::optional<double> until_render_start_variation_ms;
    std::optional<double> qpc_time;

    // Internal frame data fields; Remove for public build
    std::optional<double> since_input_ms;
    std::optional<double> since_input_variation_ms;
    std::optional<double> gpu_active_ms;
    std::optional<double> gpu_active_variation_ms;
    std::optional<double> gpu_video_active_ms;
    std::optional<double> gpu_video_active_variation_ms;

    std::optional<double> gpu_power_w;
    std::optional<double> gpu_power_variation_w;
    std::optional<double> gpu_sustained_power_limit_w;
    std::optional<double> gpu_sustained_power_limit_variation_w;
    std::optional<double> gpu_voltage_v;
    std::optional<double> gpu_voltage_variation_v;
    std::optional<double> gpu_frequency_mhz;
    std::optional<double> gpu_frequency_variation_mhz;
    std::optional<double> gpu_temp_c;
    std::optional<double> gpu_temp_variation_c;
    std::optional<double> gpu_util_percent;
    std::optional<double> gpu_util_variation_percent;
    std::optional<double> gpu_render_compute_util_percent;
    std::optional<double> gpu_render_compute_util_variation_percent;
    std::optional<double> gpu_media_util_percent;
    std::optional<double> gpu_media_util_variation_percent;
    std::optional<double> vram_power_w;
    std::optional<double> vram_power_variation_w;
    std::optional<double> vram_voltage_v;
    std::optional<double> vram_voltage_variation_v;
    std::optional<double> vram_frequency_mhz;
    std::optional<double> vram_frequency_variation_mhz;
    std::optional<double> vram_effective_frequency_gbps;
    std::optional<double> vram_effective_frequency_variation_gbps;
    std::optional<double> vram_read_bw_gbps;
    std::optional<double> vram_read_bw_variation_gbps;
    std::optional<double> vram_write_bw_gbps;
    std::optional<double> vram_write_bw_variation_gbps;
    std::optional<double> vram_temp_c;
    std::optional<double> vram_temp_variation_c;
    std::optional<uint64_t> gpu_mem_total_size_b;
    std::optional<uint64_t> gpu_mem_total_size_variation_b;
    std::optional<uint64_t> gpu_mem_used_b;
    std::optional<uint64_t> gpu_mem_used_variation_b;
    std::optional<uint64_t> gpu_mem_max_bw_gbps;
    std::optional<uint64_t> gpu_mem_max_bw_variation_gbps;
    std::optional<uint64_t> gpu_mem_read_bw_bps;
    std::optional<uint64_t> gpu_mem_read_bw_variation_bps;
    std::optional<uint64_t> gpu_mem_write_bw_bps;
    std::optional<uint64_t> gpu_mem_write_bw_variation_bps;
    std::optional<double> gpu_fan_speed_rpm;
    std::optional<double> gpu_fan_speed_rpm_variation_rpm;
    std::optional<double> gpu_power_limited_percent;
    std::optional<double> gpu_temp_limited_percent;
    std::optional<double> gpu_current_limited_percent;
    std::optional<double> gpu_voltage_limited_percent;
    std::optional<double> gpu_util_limited_percent;
    std::optional<double> vram_power_limited_percent;
    std::optional<double> vram_temp_limited_percent;
    std::optional<double> vram_current_limited_percent;
    std::optional<double> vram_voltage_limited_percent;
    std::optional<double> vram_util_limited_percent;
    std::optional<double> cpu_util_percent;
    std::optional<double> cpu_util_variation_percent;
    std::optional<double> cpu_frequency_mhz;
    std::optional<double> cpu_frequency_variation_mhz;
  };

  PmFrameGenerator(const FrameParams& frame_params);

  void GenerateFrames(int num_frames);
  size_t GetNumFrames();
  PmNsmFrameData GetFrameData(int frame_num);
  PM_FRAME_DATA GetPmFrameData(int frame_num,
                               GpuTelemetryBitset gpu_telemetry_cap_bits,
                               CpuTelemetryBitset cpu_telemetry_cap_bits);

  // Calculate the Fps metrics using the PresentMon frame data and not
  // the raw qpc data.
  bool CalculateFpsMetrics(uint32_t start_frame, double window_size_in_ms,
                           std::vector<PM_FPS_DATA>& fps_metrics);
  void CalculateLatencyMetrics(
      uint32_t start_frame, double window_size_in_ms,
      std::vector<PM_GFX_LATENCY_DATA>& latency_metrics);
  void CalculateGpuMetrics(uint32_t start_frame, double window_size_in_ms,
                           GpuTelemetryBitset gpu_telemetry_cap_bits,
                           PM_GPU_DATA& gpu_metrics);
  void CalculateCpuMetrics(uint32_t start_frame, double window_size_in_ms,
                           CpuTelemetryBitset cpu_telemetry_cap_bits,
                           PM_CPU_DATA& cpu_metrics);


  // Simple function to set the number of swap chains to be created
  // when generating  // frame data. If set AFTER frame data generation it
  // will have no affect on current data. Frames must be re-generated after
  // setting the number of swapchains. When more than one swap chain is
  // specified the swap chain will be divided as equal as possible among
  // the frames.
  void SetNumberSwapChains(uint32_t num_swap_chains) {
    swap_chains_.clear();
    swap_chains_.resize(num_swap_chains);
    for (uint32_t i = 0; i < swap_chains_.size(); i++) {
      swap_chains_[i] = (uint64_t)i;
    }
  }
  void SetFps(double fps) { between_presents_ms_ = 1000. / fps; };
  size_t GetNumberOfSwapChains() { return swap_chains_.size(); }
  double GetWindowSize() {
    return QpcDeltaToMs(
        (frames_[frames_.size() - 1].present_event.PresentStartTime -
         frames_[0].present_event.PresentStartTime),
                        qpc_frequency_);
  }
  uint32_t GetSeed() { return uniform_random_gen_.GetSeed(); }

 private:
  class UniformRandomGenerator {
   public:
    UniformRandomGenerator(uint32_t seed = std::random_device{}())
        : seed_{seed}, engine_{seed} {}
    template <Numeric T>
    T Generate(T low, T high) {
      if constexpr (std::is_integral_v<T>) {
        return std::uniform_int_distribution<T>{low, high}(engine_);
      } else {
        return std::uniform_real_distribution<T>{low, high}(engine_);
      }
    }
    uint32_t GetSeed() const { return seed_; }

   private:
    std::mt19937 engine_;
    uint32_t seed_;
  };  // note: no need to provide virtual dtor; this is not a polymorphic type
      // (no virtual functions)

  template <Numeric T>
  T GetAlteredTimingValue(T metric_in, T metric_variation_value) {
    if constexpr (std::is_unsigned<T>::value) {
      return metric_in +
                 uniform_random_gen_.Generate((T)0, metric_variation_value);
    } else {
      return metric_in +
             uniform_random_gen_.Generate(-1. * metric_variation_value,
                                          metric_variation_value);
    }
  }

  struct fps_swap_chain_data {
    std::vector<double> presented_fps;
    std::vector<double> display_fps;
    std::vector<double> frame_times_ms;
    std::vector<double> gpu_sum_ms;
    std::vector<double> dropped;
    double mLastDisplayedScreenTime = 0.;
    double display_0_screen_time = 0.;
    double cpu_n_time = 0.;
    double cpu_0_time = 0.;
    double time_in_s = 0.;
    uint32_t num_presents = 0;
    int32_t sync_interval = 0;
    PM_PRESENT_MODE present_mode = PM_PRESENT_MODE_UNKNOWN;
  };

  struct latency_swap_chain_data {
    std::vector<double> render_latency_ms;
    std::vector<double> display_latency_ms;
  };

  struct gpu_data {
    std::vector<double> gpu_power_w;
    std::vector<double> gpu_sustained_power_limit_w;
    std::vector<double> gpu_voltage_v;
    std::vector<double> gpu_frequency_mhz;
    std::vector<double> gpu_temp_c;
    std::vector<double> gpu_util_percent;
    std::vector<double> gpu_render_compute_util_percent;
    std::vector<double> gpu_media_util_percent;
    std::vector<double> vram_power_w;
    std::vector<double> vram_voltage_v;
    std::vector<double> vram_frequency_mhz;
    std::vector<double> vram_effective_frequency_gbps;
    std::vector<double> vram_read_bw_gbps;
    std::vector<double> vram_write_bw_gbps;
    std::vector<double> vram_temp_c;
    std::vector<double> gpu_mem_total_size_b;
    std::vector<double> gpu_mem_used_b;
    std::vector<double> gpu_mem_util_percent;
    std::vector<double> gpu_mem_max_bw_gbps;
    std::vector<double> gpu_mem_read_bw_bps;
    std::vector<double> gpu_mem_write_bw_bps;
    std::vector<double> gpu_fan_speed_rpm;
    std::vector<double> gpu_power_limited_percent;
    std::vector<double> gpu_temp_limited_percent;
    std::vector<double> gpu_current_limited_percent;
    std::vector<double> gpu_voltage_limited_percent;
    std::vector<double> gpu_util_limited_percent;
    std::vector<double> vram_power_limited_percent;
    std::vector<double> vram_temp_limited_percent;
    std::vector<double> vram_current_limited_percent;
    std::vector<double> vram_voltage_limited_percent;
    std::vector<double> vram_util_limited_percent;
  };

  struct cpu_data {
    std::vector<double> cpu_util_percent;
    std::vector<double> cpu_frequency_mhz;
  };

  void GeneratePresentData();
  void GenerateGPUData();
  void GenerateCPUData();

  void CalcMetricStats(std::vector<double>& data,
                       PM_METRIC_DOUBLE_DATA& metric,
                       bool valid=true);

  // Calculate percentile using linear interpolation between the closet ranks
  // method
  double GetPercentile(std::vector<double>& data, double percentile);
  bool IsLimited(const double& limited_percentage) {
    if (uniform_random_gen_.Generate(0., 100.) < limited_percentage) {
      return true;
    } else {
      return false;
    }
  }

  std::string app_name_;
  std::vector<uint64_t> swap_chains_;
  uint32_t process_id_;
  Runtime runtime_;
  int32_t sync_interval_;
  uint32_t present_flags_;
  double percent_dropped_;
  double in_present_api_ms_;
  double in_present_api_variation_ms_;
  double between_presents_ms_;
  double between_presents_variation_ms_;
  double percent_tearing_;
  PresentMode present_mode_;
  double until_render_complete_ms_;
  double until_render_complete_variation_ms_;
  double until_displayed_ms_;
  double until_displayed_variation_ms_;
  double until_display_change_ms_;
  double until_display_change_variation_ms_;
  double until_render_start_ms_;
  double until_render_start_variation_ms_;
  double qpc_time_;
  double since_input_ms_;
  double since_input_variation_ms_;
  double gpu_active_ms_;
  double gpu_active_variation_ms_;
  double gpu_video_active_ms_;
  double gpu_video_active_variation_ms_;

  double gpu_power_w_;
  double gpu_power_variation_w_;
  double gpu_sustained_power_limit_w_;
  double gpu_sustained_power_limit_variation_w_;
  double gpu_voltage_v_;
  double gpu_voltage_variation_v_;
  double gpu_frequency_mhz_;
  double gpu_frequency_variation_mhz_;
  double gpu_temp_c_;
  double gpu_temp_variation_c_;
  double gpu_util_percent_;
  double gpu_util_variation_percent_;
  double gpu_render_compute_util_percent_;
  double gpu_render_compute_util_variation_percent_;
  double gpu_media_util_percent_;
  double gpu_media_util_variation_percent_;
  double vram_power_w_;
  double vram_power_variation_w_;
  double vram_voltage_v_;
  double vram_voltage_variation_v_;
  double vram_frequency_mhz_;
  double vram_frequency_variation_mhz_;
  double vram_effective_frequency_gbps_;
  double vram_effective_frequency_variation_gbps_;
  double vram_temp_c_;
  double vram_temp_variation_c_;
  uint64_t gpu_mem_total_size_b_;
  uint64_t gpu_mem_total_size_variation_b_;
  uint64_t gpu_mem_used_b_;
  uint64_t gpu_mem_used_variation_b_;
  uint64_t gpu_mem_max_bw_gbps_;
  uint64_t gpu_mem_max_bw_variation_gbps_;
  uint64_t gpu_mem_read_bw_bps_;
  uint64_t gpu_mem_read_bw_variation_bps_;
  uint64_t gpu_mem_write_bw_bps_;
  uint64_t gpu_mem_write_bw_variation_bps_;
  double gpu_fan_speed_rpm_;
  double gpu_fan_speed_rpm_variation_rpm_;
  double gpu_power_limited_percent_;
  double gpu_temp_limited_percent_;
  double gpu_current_limited_percent_;
  double gpu_voltage_limited_percent_;
  double gpu_util_limited_percent_;
  double vram_power_limited_percent_;
  double vram_temp_limited_percent_;
  double vram_current_limited_percent_;
  double vram_voltage_limited_percent_;
  double vram_util_limited_percent_;
  double cpu_util_percent_;
  double cpu_util_variation_percent_;
  double cpu_frequency_mhz_;
  double cpu_frequency_variation_mhz_;
  LARGE_INTEGER qpc_frequency_;
  LARGE_INTEGER start_qpc_;

  std::vector<PmNsmFrameData> frames_;
  std::vector<PMFrameTimingInformation> pmft_frames_;
  UniformRandomGenerator uniform_random_gen_;
};