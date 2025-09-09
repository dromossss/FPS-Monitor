#include "../PresentMonUtils/MemBuffer.h"
#include "../PresentMonUtils/PresentMonNamedPipe.h"
#include "../Streamer/StreamClient.h"
#include "../Streamer/Streamer.h"
#include "PmFrameGenerator.h"
#include "gtest/gtest.h"
#include "utils.h"

#include "../CommonUtilities/log/GlogShim.h"

const uint32_t kPid = 10;
const double kRunWindowSize = 1000.0f;
const uint32_t kNumFrames = 150;

// Error tolerance
const double kErrorTolerance = 0.2;

static bool fltCmp(double val1, double val2, double tolerance) {
  if (fabs(val1 - val2) < tolerance) {
    return true;
  } else {
    return false;
  }
}

class PresentMonApiULT : public ::testing::Test {};

bool Equal(const PM_METRIC_DOUBLE_DATA &data1, const PM_METRIC_DOUBLE_DATA& data2) {

  if (!fltCmp(data1.avg, data2.avg, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.high, data2.high, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.low, data2.low, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.percentile_90, data2.percentile_90, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.percentile_95, data2.percentile_95, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.percentile_95, data2.percentile_95, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.raw, data2.raw, kErrorTolerance)) {
    return false;
  }
  return true;
} 

template <Numeric T>
bool CompareTelemetry(T telemetry_a, T telemetry_b) {
  if constexpr (std::is_floating_point<T>::value) {
    return (fltCmp(telemetry_a, telemetry_b, kErrorTolerance));
  } else {
    return (telemetry_a == telemetry_b);
  }
}

template <typename T>
bool CompareOptionalTelemetry(T telemetry_a, T telemetry_b) {
  
  if (telemetry_a.valid == telemetry_b.valid) {
    return CompareTelemetry(telemetry_a.data, telemetry_b.data);
    return true;
  } else {
    return false;
  }
}

bool CompareFrameData(const PM_FRAME_DATA &data1, const PM_FRAME_DATA &data2) {
  std::string data1_string = data1.application;
  std::string data2_string = data2.application;
  if (data1_string != data2_string) {
    return false;
  }
  if (data1.swap_chain_address != data2.swap_chain_address) {
    return false;
  }
  data1_string = data1.runtime;
  data2_string = data2.runtime;
  if (data1_string != data2_string) {
    return false;
  }
  if (data1.sync_interval != data2.sync_interval) {
    return false;
  }
  if (data1.present_flags != data2.present_flags) {
    return false;
  }
  if (data1.dropped != data2.dropped) {
    return false;
  }
  if (!fltCmp(data1.time_in_seconds, data2.time_in_seconds, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_in_present_api, data2.ms_in_present_api,
              kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_between_presents, data2.ms_between_presents,
              kErrorTolerance)) {
    return false;
  }
  if (data1.allows_tearing != data2.allows_tearing) {
    return false;
  }
  if (data1.present_mode != data2.present_mode) {
    return false;
  }
  if (!fltCmp(data1.ms_until_render_complete, data2.ms_until_render_complete,
              kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_until_displayed, data2.ms_until_displayed,
              kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_between_display_change, data2.ms_between_display_change,
              kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_until_render_start, data2.ms_until_render_start,
              kErrorTolerance)) {
    return false;
  }
  if (data1.qpc_time != data2.qpc_time) {
    return false;
  }

  if (!fltCmp(data1.ms_since_input, data2.ms_since_input, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_gpu_active, data2.ms_gpu_active, kErrorTolerance)) {
    return false;
  }
  if (!fltCmp(data1.ms_gpu_video_active, data2.ms_gpu_video_active, kErrorTolerance)) {
    return false;
  }

  // Power Telemetry data
  if (!CompareOptionalTelemetry(data1.gpu_power_w, data2.gpu_power_w)){
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_sustained_power_limit_w,
              data2.gpu_sustained_power_limit_w)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_voltage_v, data2.gpu_voltage_v)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_frequency_mhz, data2.gpu_frequency_mhz)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_temperature_c, data2.gpu_temperature_c)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_utilization, data2.gpu_utilization)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_render_compute_utilization,
              data2.gpu_render_compute_utilization)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_media_utilization, data2.gpu_media_utilization)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_power_w, data2.vram_power_w)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_voltage_v, data2.vram_voltage_v)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_frequency_mhz, data2.vram_frequency_mhz)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_effective_frequency_gbs,
              data2.vram_effective_frequency_gbs)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_temperature_c, data2.vram_temperature_c)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_mem_total_size_b, data2.gpu_mem_total_size_b)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_mem_used_b, data2.gpu_mem_used_b)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_mem_max_bandwidth_bps, data2.gpu_mem_max_bandwidth_bps)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_mem_read_bandwidth_bps,
              data2.gpu_mem_read_bandwidth_bps)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_mem_write_bandwidth_bps,
              data2.gpu_mem_write_bandwidth_bps)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_power_limited, data2.gpu_power_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_temperature_limited, data2.gpu_temperature_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_current_limited, data2.gpu_current_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_voltage_limited, data2.gpu_voltage_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.gpu_utilization_limited, data2.gpu_utilization_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_power_limited, data2.vram_power_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_temperature_limited, data2.vram_temperature_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_current_limited, data2.vram_current_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_voltage_limited, data2.vram_voltage_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.vram_utilization_limited, data2.vram_utilization_limited)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.cpu_utilization, data2.cpu_utilization)) {
    return false;
  }
  if (!CompareOptionalTelemetry(data1.cpu_frequency, data2.cpu_frequency)) {
    return false;
  }

  return true;
}

TEST_F(PresentMonApiULT, TestFpsMetrics) {

  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestFpsMetrics test: "
            << frame_gen.GetSeed();
  frame_gen.SetFps(60.);
  frame_gen.GenerateFrames(kNumFrames);
  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  {
    Streamer test_streamer;
    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    std::vector<PM_FPS_DATA> calculated_fps_data;
    frame_gen.CalculateFpsMetrics((uint32_t)frame_gen.GetNumFrames(),
                                   kRunWindowSize, calculated_fps_data);

    PM_FPS_DATA fps_data{};
    uint32_t num_swap_chains = 1;
    pm_client.GetFramesPerSecondData(kPid, &fps_data, kRunWindowSize,
                                     &num_swap_chains);
    EXPECT_TRUE(1 == num_swap_chains);
    EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_fps_data.size()));

    EXPECT_TRUE(
        Equal(fps_data.presented_fps, calculated_fps_data[0].presented_fps));
    EXPECT_TRUE(
        Equal(fps_data.displayed_fps, calculated_fps_data[0].displayed_fps));
    EXPECT_TRUE(
        Equal(fps_data.frame_time_ms, calculated_fps_data[0].frame_time_ms));
    EXPECT_TRUE(Equal(fps_data.gpu_busy, calculated_fps_data[0].gpu_busy));
    EXPECT_TRUE(Equal(fps_data.percent_dropped_frames,
                      calculated_fps_data[0].percent_dropped_frames));

    EXPECT_TRUE((fps_data.sync_interval == 0));
    EXPECT_TRUE(fps_data.present_mode == calculated_fps_data[0].present_mode);

    EXPECT_TRUE((fps_data.num_presents == calculated_fps_data[0].num_presents));
  }

  frame_gen.SetFps(300.);
  frame_gen.GenerateFrames(kNumFrames);
  {
    Streamer test_streamer;
    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    std::vector<PM_FPS_DATA> calculated_fps_data;
    frame_gen.CalculateFpsMetrics((uint32_t)frame_gen.GetNumFrames(),
                                   kRunWindowSize, calculated_fps_data);

    PM_FPS_DATA fps_data{};
    uint32_t num_swap_chains = 1;
    pm_client.GetFramesPerSecondData(kPid, &fps_data, kRunWindowSize,
                                     &num_swap_chains);
    EXPECT_TRUE(1 == num_swap_chains);
    EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_fps_data.size()));

    EXPECT_TRUE(
        Equal(fps_data.presented_fps, calculated_fps_data[0].presented_fps));
    EXPECT_TRUE(
        Equal(fps_data.displayed_fps, calculated_fps_data[0].displayed_fps));
    EXPECT_TRUE(
        Equal(fps_data.frame_time_ms, calculated_fps_data[0].frame_time_ms));

    EXPECT_TRUE(Equal(fps_data.gpu_busy, calculated_fps_data[0].gpu_busy));
    EXPECT_TRUE(Equal(fps_data.percent_dropped_frames,
                      calculated_fps_data[0].percent_dropped_frames));

    EXPECT_TRUE((fps_data.sync_interval == 0));
    EXPECT_TRUE(fps_data.present_mode == calculated_fps_data[0].present_mode);

    EXPECT_TRUE((fps_data.num_presents == calculated_fps_data[0].num_presents));
  }
}

TEST_F(PresentMonApiULT, TestFpsMetricsMultiSwapChains) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestFpsMetricsMultiSwapChains test: "
            << frame_gen.GetSeed();
  frame_gen.SetFps(180.);
  frame_gen.SetNumberSwapChains(3);
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  std::vector<PM_FPS_DATA> calculated_fps_data;
  frame_gen.CalculateFpsMetrics((uint32_t)frame_gen.GetNumFrames(),
                                 kRunWindowSize, calculated_fps_data);

  PM_FPS_DATA fps_data[3]{};
  uint32_t num_swap_chains = 3;
  pm_client.GetFramesPerSecondData(kPid, fps_data, kRunWindowSize,
                                   &num_swap_chains);
  EXPECT_TRUE(3 == num_swap_chains);
  EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_fps_data.size()));

  for (uint32_t i = 0; i < num_swap_chains; i++) {
    EXPECT_TRUE(
        Equal(fps_data[i].presented_fps, calculated_fps_data[i].presented_fps));
    EXPECT_TRUE(
        Equal(fps_data[i].displayed_fps, calculated_fps_data[i].displayed_fps));
    EXPECT_TRUE(
        Equal(fps_data[i].frame_time_ms, calculated_fps_data[i].frame_time_ms));
    EXPECT_TRUE(Equal(fps_data[i].gpu_busy, calculated_fps_data[i].gpu_busy));
    EXPECT_TRUE(Equal(fps_data[i].percent_dropped_frames,
                      calculated_fps_data[i].percent_dropped_frames));

    EXPECT_TRUE((fps_data[i].sync_interval == 0));
    EXPECT_TRUE(fps_data[i].present_mode ==
                calculated_fps_data[i].present_mode);

    EXPECT_TRUE(
        (fps_data[i].num_presents == calculated_fps_data[i].num_presents));
  }
}

TEST_F(PresentMonApiULT, TestGfxLatencyMultiSwapChains) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for GetFramesPerSecondData test: "
            << frame_gen.GetSeed();
  frame_gen.SetFps(60.);
  frame_gen.SetNumberSwapChains(4);
  frame_gen.GenerateFrames(kNumFrames);
  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  {
    Streamer test_streamer;
    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    std::vector<PM_GFX_LATENCY_DATA> calculated_latency_data;
    frame_gen.CalculateLatencyMetrics((uint32_t)frame_gen.GetNumFrames(),
                                       kRunWindowSize, calculated_latency_data);

    PM_GFX_LATENCY_DATA gfx_latency_data[4]{};
    uint32_t num_swap_chains = 4;
    pm_client.GetGfxLatencyData(kPid, gfx_latency_data, kRunWindowSize,
                                     &num_swap_chains);
    EXPECT_TRUE(4 == num_swap_chains);
    EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_latency_data.size()));

    for (uint32_t i = 0; i < num_swap_chains; i++) {
      EXPECT_TRUE(Equal(gfx_latency_data[i].display_latency_ms,
                        calculated_latency_data[i].display_latency_ms));
      EXPECT_TRUE(Equal(gfx_latency_data[i].render_latency_ms,
                        calculated_latency_data[i].render_latency_ms));
    }
  }
}

TEST_F(PresentMonApiULT, TestFpsDroppedFrames) {
  PmFrameGenerator frame_gen{
      PmFrameGenerator::FrameParams{.percent_dropped = 50.}};
  LOG(INFO) << "\nSeed used for GetFramesPerSecondData test: "
            << frame_gen.GetSeed();
  frame_gen.SetFps(302.);
  frame_gen.GenerateFrames(kNumFrames);
  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  {
    Streamer test_streamer;
    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    std::vector<PM_FPS_DATA> calculated_fps_data;
    frame_gen.CalculateFpsMetrics((uint32_t)frame_gen.GetNumFrames(),
                                   kRunWindowSize, calculated_fps_data);

    PM_FPS_DATA fps_data{};
    uint32_t num_swap_chains = 1;
    pm_client.GetFramesPerSecondData(kPid, &fps_data, kRunWindowSize,
                                     &num_swap_chains);
    EXPECT_TRUE(1 == num_swap_chains);
    EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_fps_data.size()));

    EXPECT_TRUE(
        Equal(fps_data.presented_fps, calculated_fps_data[0].presented_fps));
    EXPECT_TRUE(
        Equal(fps_data.displayed_fps, calculated_fps_data[0].displayed_fps));
    EXPECT_TRUE(
        Equal(fps_data.frame_time_ms, calculated_fps_data[0].frame_time_ms));
    EXPECT_TRUE(Equal(fps_data.gpu_busy, calculated_fps_data[0].gpu_busy));
    EXPECT_TRUE(Equal(fps_data.percent_dropped_frames,
                      calculated_fps_data[0].percent_dropped_frames));

    EXPECT_TRUE((fps_data.sync_interval == 0));
    EXPECT_TRUE(fps_data.present_mode == calculated_fps_data[0].present_mode);

    EXPECT_TRUE((fps_data.num_presents == calculated_fps_data[0].num_presents));
  }
}

TEST_F(PresentMonApiULT, TestGfxLatency) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for GetGfxLatencyData test: "
            << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  std::vector<PM_GFX_LATENCY_DATA> calculated_latency_data;
  frame_gen.CalculateLatencyMetrics((uint32_t)frame_gen.GetNumFrames(),
                                     kRunWindowSize,
                                     calculated_latency_data);

  PM_GFX_LATENCY_DATA gfx_latency_data{};
  uint32_t num_swap_chains = 1;
  pm_client.GetGfxLatencyData(kPid, &gfx_latency_data,
                              kRunWindowSize, &num_swap_chains);
  EXPECT_TRUE(1 == num_swap_chains);
  EXPECT_TRUE(num_swap_chains == (uint32_t)calculated_latency_data.size());
  EXPECT_TRUE(Equal(gfx_latency_data.display_latency_ms,
                    calculated_latency_data[0].display_latency_ms));
  EXPECT_TRUE(Equal(gfx_latency_data.render_latency_ms,
                    calculated_latency_data[0].render_latency_ms));
}

TEST_F(PresentMonApiULT, TestGfxLatencyDroppedFrames) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{.percent_dropped = 50.}};
  LOG(INFO) << "\nSeed used for GetFramesPerSecondData test: "
            << frame_gen.GetSeed();
  frame_gen.SetFps(302.);
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);
  std::vector<PM_GFX_LATENCY_DATA> calculated_latency_data;
  frame_gen.CalculateLatencyMetrics((uint32_t)frame_gen.GetNumFrames(),
                                     kRunWindowSize, calculated_latency_data);

  PM_GFX_LATENCY_DATA gfx_latency_data{};
  uint32_t num_swap_chains = 1;
  pm_client.GetGfxLatencyData(kPid, &gfx_latency_data, kRunWindowSize,
                              &num_swap_chains);
  EXPECT_TRUE(1 == num_swap_chains);
  EXPECT_TRUE(num_swap_chains == (uint32_t)calculated_latency_data.size());
  EXPECT_TRUE(Equal(gfx_latency_data.display_latency_ms,
                    calculated_latency_data[0].display_latency_ms));
  EXPECT_TRUE(Equal(gfx_latency_data.render_latency_ms,
                    calculated_latency_data[0].render_latency_ms));
}


TEST_F(PresentMonApiULT, TestSingleEntryFpsPercentiles) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestSingleEntryFpsPercentiles test: "
            << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  std::vector<PM_FPS_DATA> calculated_fps_data;
  frame_gen.CalculateFpsMetrics((uint32_t)frame_gen.GetNumFrames(), 8,
                                 calculated_fps_data);

  PM_FPS_DATA fps_data;
  uint32_t num_swap_chains = 1;
  pm_client.GetFramesPerSecondData(10, &fps_data, 8, &num_swap_chains);
  EXPECT_TRUE(1 == num_swap_chains);
  EXPECT_TRUE((num_swap_chains == (uint32_t)calculated_fps_data.size()));

  EXPECT_TRUE(fltCmp(fps_data.presented_fps.percentile_99,
                     calculated_fps_data[0].presented_fps.percentile_99,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(fps_data.presented_fps.percentile_95,
                     calculated_fps_data[0].presented_fps.percentile_95,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(fps_data.presented_fps.percentile_90,
                     calculated_fps_data[0].presented_fps.percentile_90,
                     kErrorTolerance));

  EXPECT_TRUE(fltCmp(fps_data.displayed_fps.percentile_99,
                     calculated_fps_data[0].displayed_fps.percentile_99,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(fps_data.displayed_fps.percentile_95,
                     calculated_fps_data[0].displayed_fps.percentile_95,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(fps_data.displayed_fps.percentile_90,
                     calculated_fps_data[0].displayed_fps.percentile_90,
                     kErrorTolerance));
}

TEST_F(PresentMonApiULT, TestSingleEntryGfxLatencyPercentiles) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestSingleEntryLatencyPercentiles test: "
            << frame_gen.GetSeed();
  frame_gen.GenerateFrames(20);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  std::vector<PM_GFX_LATENCY_DATA> calculated_latency_data;
  frame_gen.CalculateLatencyMetrics((uint32_t)frame_gen.GetNumFrames(),
                                     8,
                                     calculated_latency_data);

  PM_GFX_LATENCY_DATA gfx_latency_data;
  uint32_t num_swap_chains = 1;
  pm_client.GetGfxLatencyData(kPid, &gfx_latency_data, 8,
                              &num_swap_chains);
  EXPECT_EQ(1, num_swap_chains);
  EXPECT_EQ(num_swap_chains, (uint32_t)calculated_latency_data.size());
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.avg,
                     calculated_latency_data[0].display_latency_ms.avg,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.percentile_99,
             calculated_latency_data[0].display_latency_ms.percentile_99,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.percentile_95,
             calculated_latency_data[0].display_latency_ms.percentile_95,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.percentile_90,
             calculated_latency_data[0].display_latency_ms.percentile_90,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.high,
                     calculated_latency_data[0].display_latency_ms.high,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.display_latency_ms.low,
                     calculated_latency_data[0].display_latency_ms.low,
                     kErrorTolerance));

  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.avg,
                     calculated_latency_data[0].render_latency_ms.avg,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.percentile_99,
                     calculated_latency_data[0].render_latency_ms.percentile_99,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.percentile_95,
                     calculated_latency_data[0].render_latency_ms.percentile_95,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.percentile_90,
                     calculated_latency_data[0].render_latency_ms.percentile_90,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.high,
                     calculated_latency_data[0].render_latency_ms.high,
                     kErrorTolerance));
  EXPECT_TRUE(fltCmp(gfx_latency_data.render_latency_ms.low,
                     calculated_latency_data[0].render_latency_ms.low,
                     kErrorTolerance));
}

TEST_F(PresentMonApiULT, TestGpuMetrics) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{.gpu_power_limited_percent = 50.}};
  LOG(INFO) << "\nSeed used for TestGpuMetrics test: " << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  Streamer test_streamer;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  PM_GPU_DATA calculated_gpu_data{};
  frame_gen.CalculateGpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                kRunWindowSize, gpu_telemetry_cap_bits,
                                calculated_gpu_data);

  PM_GPU_DATA gfx_gpu_data{};
  pm_client.GetGpuData(kPid, &gfx_gpu_data, kRunWindowSize);

  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_power_w, gfx_gpu_data.gpu_power_w));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_sustained_power_limit_w,
                    gfx_gpu_data.gpu_sustained_power_limit_w));
  EXPECT_TRUE(
      Equal(calculated_gpu_data.gpu_voltage_v, gfx_gpu_data.gpu_voltage_v));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_frequency_mhz,
                    gfx_gpu_data.gpu_frequency_mhz));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_temperature_c,
                    gfx_gpu_data.gpu_temperature_c));
  EXPECT_TRUE(
      Equal(calculated_gpu_data.gpu_utilization, gfx_gpu_data.gpu_utilization));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_render_compute_utilization,
                    gfx_gpu_data.gpu_render_compute_utilization));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_media_utilization,
                    gfx_gpu_data.gpu_media_utilization));
  EXPECT_TRUE(
      Equal(calculated_gpu_data.vram_power_w, gfx_gpu_data.vram_power_w));
  EXPECT_TRUE(
      Equal(calculated_gpu_data.vram_voltage_v, gfx_gpu_data.vram_voltage_v));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_frequency_mhz,
                    gfx_gpu_data.vram_frequency_mhz));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_effective_frequency_gbps,
                    gfx_gpu_data.vram_effective_frequency_gbps));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_read_bandwidth_bps,
                    gfx_gpu_data.vram_read_bandwidth_bps));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_write_bandwidth_bps,
                    gfx_gpu_data.vram_write_bandwidth_bps));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_temperature_c,
                    gfx_gpu_data.vram_temperature_c));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_mem_total_size_b,
                    gfx_gpu_data.gpu_mem_total_size_b));
  EXPECT_TRUE(
      Equal(calculated_gpu_data.gpu_mem_used_b, gfx_gpu_data.gpu_mem_used_b));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_mem_utilization,
                    gfx_gpu_data.gpu_mem_utilization));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_mem_max_bandwidth_bps,
                    gfx_gpu_data.gpu_mem_max_bandwidth_bps));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_mem_read_bandwidth_bps,
                    gfx_gpu_data.gpu_mem_read_bandwidth_bps));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_mem_write_bandwidth_bps,
                    gfx_gpu_data.gpu_mem_write_bandwidth_bps));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_power_limited,
                    gfx_gpu_data.gpu_power_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_temperature_limited,
                    gfx_gpu_data.gpu_temperature_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_current_limited,
                    gfx_gpu_data.gpu_current_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_voltage_limited,
                    gfx_gpu_data.gpu_voltage_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.gpu_utilization_limited,
                    gfx_gpu_data.gpu_utilization_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_power_limited,
                    gfx_gpu_data.vram_power_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_temperature_limited,
                    gfx_gpu_data.vram_temperature_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_current_limited,
                    gfx_gpu_data.vram_current_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_voltage_limited,
                    gfx_gpu_data.vram_voltage_limited));
  EXPECT_TRUE(Equal(calculated_gpu_data.vram_utilization_limited,
                    gfx_gpu_data.vram_utilization_limited));
  for (int i = 0; i < MAX_PM_FAN_COUNT; i++) {
    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_fan_speed_rpm[i],
                      gfx_gpu_data.gpu_fan_speed_rpm[i]));
  }
}

TEST_F(PresentMonApiULT, TestTelemetryCapBits) {
  PmFrameGenerator frame_gen{
      PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestTelemetryCapBits test: " << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  Streamer test_streamer;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  {
    // Both the GPU and CPU metrics tests enable all of the telemetry bits.
    // In this test only enable a single GPU and CPU telemetry item
    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(GpuTelemetryCapBits::gpu_utilization), true);
    cpu_telemetry_cap_bits.set(
        static_cast<size_t>(CpuTelemetryCapBits::cpu_utilization), true);
    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    PM_GPU_DATA calculated_gpu_data{};
    frame_gen.CalculateGpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                  kRunWindowSize, gpu_telemetry_cap_bits,
                                  calculated_gpu_data);
    PM_GPU_DATA gfx_gpu_data{};
    pm_client.GetGpuData(kPid, &gfx_gpu_data, kRunWindowSize);

    // Check to make sure only GPU Utilization is valid, has the correct values
    // and that all other telemetry items are not valid
    EXPECT_EQ(gfx_gpu_data.gpu_utilization.valid, true);
    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_utilization,
                      gfx_gpu_data.gpu_utilization));
    EXPECT_EQ(gfx_gpu_data.gpu_power_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_sustained_power_limit_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_voltage_v.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_frequency_mhz.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_temperature_c.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_render_compute_utilization.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_media_utilization.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_power_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_voltage_v.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_frequency_mhz.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_effective_frequency_gbps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_read_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_write_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_temperature_c.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_total_size_b.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_used_b.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_utilization.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_max_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_read_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_write_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_power_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_temperature_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_current_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_voltage_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_utilization_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_power_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_temperature_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_current_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_voltage_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_utilization_limited.valid, false);
    for (int i = 0; i < MAX_PM_FAN_COUNT; i++) {
      EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[i].valid, false);
    }
    // Now inspect CPU telemetry. Again a single telemetry should
    // be valid (CPU Utilization).
    PM_CPU_DATA calculated_cpu_data{};
    frame_gen.CalculateCpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                  kRunWindowSize, cpu_telemetry_cap_bits,
                                  calculated_cpu_data);
    PM_CPU_DATA cpu_data{};
    pm_client.GetCpuData(kPid, &cpu_data, kRunWindowSize);
    EXPECT_EQ(cpu_data.cpu_utilization.valid, true);
    EXPECT_TRUE(
        Equal(calculated_cpu_data.cpu_utilization, cpu_data.cpu_utilization));
    EXPECT_EQ(cpu_data.cpu_power_w.valid, false);
    EXPECT_EQ(cpu_data.cpu_power_limit_w.valid, false);
    EXPECT_EQ(cpu_data.cpu_temperature_c.valid, false);
    EXPECT_EQ(cpu_data.cpu_frequency.valid, false);
  }

  {
    gpu_telemetry_cap_bits.reset();
    cpu_telemetry_cap_bits.reset();

    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(GpuTelemetryCapBits::gpu_utilization), true);
    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(
            GpuTelemetryCapBits::gpu_render_compute_utilization),
        true);
    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(GpuTelemetryCapBits::gpu_media_utilization),
        true);
    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(GpuTelemetryCapBits::fan_speed_2), true);
    gpu_telemetry_cap_bits.set(
        static_cast<size_t>(GpuTelemetryCapBits::vram_power_limited), true);
    cpu_telemetry_cap_bits.set(
        static_cast<size_t>(CpuTelemetryCapBits::cpu_frequency), true);

    test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
    for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
      auto frame = frame_gen.GetFrameData(i);
      test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                   cpu_telemetry_cap_bits);
    }

    PresentMonClient pm_client;
    std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
    client->Initialize(test_streamer.GetMapFileName(kPid));
    pm_client.clients_.emplace(kPid, std::move(client));
    pm_client.SetupClientCaches(kPid);

    EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

    PM_GPU_DATA calculated_gpu_data{};
    frame_gen.CalculateGpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                  kRunWindowSize, gpu_telemetry_cap_bits,
                                  calculated_gpu_data);
    PM_GPU_DATA gfx_gpu_data{};
    pm_client.GetGpuData(kPid, &gfx_gpu_data, kRunWindowSize);

    EXPECT_EQ(gfx_gpu_data.gpu_utilization.valid, true);
    EXPECT_EQ(gfx_gpu_data.gpu_render_compute_utilization.valid, true);
    EXPECT_EQ(gfx_gpu_data.gpu_media_utilization.valid, true);
    EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[2].valid, true);
    EXPECT_EQ(gfx_gpu_data.vram_power_limited.valid, true);

    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_utilization,
                      gfx_gpu_data.gpu_utilization));
    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_render_compute_utilization,
                      gfx_gpu_data.gpu_render_compute_utilization));
    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_media_utilization,
                      gfx_gpu_data.gpu_media_utilization));
    EXPECT_TRUE(Equal(calculated_gpu_data.gpu_fan_speed_rpm[2],
                      gfx_gpu_data.gpu_fan_speed_rpm[2]));
    EXPECT_TRUE(Equal(calculated_gpu_data.vram_power_limited,
                      gfx_gpu_data.vram_power_limited));

    EXPECT_EQ(gfx_gpu_data.gpu_power_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_sustained_power_limit_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_voltage_v.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_frequency_mhz.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_temperature_c.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_power_w.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_voltage_v.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_frequency_mhz.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_effective_frequency_gbps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_read_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_write_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_temperature_c.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_total_size_b.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_used_b.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_utilization.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_max_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_read_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_mem_write_bandwidth_bps.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_power_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_temperature_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_current_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_voltage_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_utilization_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_temperature_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_current_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_voltage_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.vram_utilization_limited.valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[0].valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[1].valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[3].valid, false);
    EXPECT_EQ(gfx_gpu_data.gpu_fan_speed_rpm[4].valid, false);

    // Now inspect CPU telemetry. Again a single telemetry should
    // be valid (CPU Utilization).
    PM_CPU_DATA calculated_cpu_data{};
    frame_gen.CalculateCpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                  kRunWindowSize, cpu_telemetry_cap_bits,
                                  calculated_cpu_data);
    PM_CPU_DATA cpu_data{};
    pm_client.GetCpuData(kPid, &cpu_data, kRunWindowSize);
    EXPECT_EQ(cpu_data.cpu_frequency.valid, true);
    EXPECT_TRUE(
        Equal(calculated_cpu_data.cpu_frequency, cpu_data.cpu_frequency));
    EXPECT_EQ(cpu_data.cpu_power_w.valid, false);
    EXPECT_EQ(cpu_data.cpu_power_limit_w.valid, false);
    EXPECT_EQ(cpu_data.cpu_temperature_c.valid, false);
    EXPECT_EQ(cpu_data.cpu_utilization.valid, false);
  }
}


TEST_F(PresentMonApiULT, TestCpuMetrics) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestCpuMetrics test: " << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();
  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);
  for (int i = 0; i <= frame_gen.GetNumFrames(); i++) {
    auto frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  pm_client.SetupClientCaches(kPid);

  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  PM_CPU_DATA calculated_cpu_data{};
  frame_gen.CalculateCpuMetrics((uint32_t)frame_gen.GetNumFrames(),
                                kRunWindowSize, cpu_telemetry_cap_bits,
                                calculated_cpu_data);

  PM_CPU_DATA cpu_data{};
  pm_client.GetCpuData(kPid, &cpu_data, kRunWindowSize);

  EXPECT_TRUE(Equal(calculated_cpu_data.cpu_utilization, cpu_data.cpu_utilization));
  EXPECT_TRUE(Equal(calculated_cpu_data.cpu_frequency, cpu_data.cpu_frequency));
}

TEST_F(PresentMonApiULT, TestFrameCapture) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestFrameCapture test: " << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;

  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  auto frame_data = std::make_unique<PM_FRAME_DATA[]>(kNumFrames);
  uint32_t num_frames = kNumFrames;
  auto pm_status = pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == 0);
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_NO_DATA);
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  gpu_telemetry_cap_bits.set();
  cpu_telemetry_cap_bits.set();

  // Stream a single frame.
  auto frame = frame_gen.GetFrameData(0);
  test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                               cpu_telemetry_cap_bits);
  num_frames = kNumFrames;
  // This first call initializes the frame data capture system on the pm
  // client side. No frames will be captured on the first call
  pm_status = pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == 0);
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_NO_DATA);

  // Stream the rest of the generated frames
  for (int i = 1; i <= frame_gen.GetNumFrames(); i++) {
    frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }
  num_frames = kNumFrames;
  pm_status = pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == kNumFrames - 1);
  EXPECT_TRUE(num_frames == (uint32_t)frame_gen.GetNumFrames());
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_SUCCESS);
  // Compare all recorded frames
  for (uint32_t i = 0; i <= ((uint32_t)frame_gen.GetNumFrames() - 1); i++) {
    EXPECT_TRUE(CompareFrameData(
        frame_data[i], frame_gen.GetPmFrameData(i, gpu_telemetry_cap_bits,
                                                cpu_telemetry_cap_bits)));
  }
  
}

TEST_F(PresentMonApiULT, TestFrameCaptureCapBits) {
  PmFrameGenerator frame_gen{PmFrameGenerator::FrameParams{}};
  LOG(INFO) << "\nSeed used for TestFrameCapture test: " << frame_gen.GetSeed();
  frame_gen.GenerateFrames(kNumFrames);

  DWORD client_process_id = GetCurrentProcessId();
  std::string nsm_name;

  Streamer test_streamer;
  test_streamer.StartStreaming(client_process_id, kPid, nsm_name);

  PresentMonClient pm_client;
  std::unique_ptr<StreamClient> client = std::make_unique<StreamClient>();
  client->Initialize(test_streamer.GetMapFileName(kPid));
  pm_client.clients_.emplace(kPid, std::move(client));
  EXPECT_NE(pm_client.clients_[kPid]->GetNamedSharedMemView(), nullptr);

  auto frame_data = std::make_unique<PM_FRAME_DATA[]>(kNumFrames);
  uint32_t num_frames = kNumFrames;
  auto pm_status =
      pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == 0);
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_NO_DATA);
  GpuTelemetryBitset gpu_telemetry_cap_bits;
  CpuTelemetryBitset cpu_telemetry_cap_bits;
  // Both the GPU and CPU metrics tests enable all of the telemetry bits.
  // In this test only enable a few metrics
  gpu_telemetry_cap_bits.set(
      static_cast<size_t>(GpuTelemetryCapBits::gpu_power), true);
  gpu_telemetry_cap_bits.set(
      static_cast<size_t>(GpuTelemetryCapBits::gpu_frequency), true);
  gpu_telemetry_cap_bits.set(
      static_cast<size_t>(GpuTelemetryCapBits::gpu_utilization), true);
  gpu_telemetry_cap_bits.set(
      static_cast<size_t>(GpuTelemetryCapBits::gpu_mem_size), true);
  gpu_telemetry_cap_bits.set(
      static_cast<size_t>(GpuTelemetryCapBits::gpu_mem_used), true);
  cpu_telemetry_cap_bits.set(
      static_cast<size_t>(CpuTelemetryCapBits::cpu_utilization), true);

  // Stream a single frame.
  auto frame = frame_gen.GetFrameData(0);
  test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                               cpu_telemetry_cap_bits);
  num_frames = kNumFrames;
  // This first call initializes the frame data capture system on the pm
  // client side. No frames will be captured on the first call
  pm_status =
      pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == 0);
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_NO_DATA);

  // Stream the rest of the generated frames
  for (int i = 1; i <= frame_gen.GetNumFrames(); i++) {
    frame = frame_gen.GetFrameData(i);
    test_streamer.WriteFrameData(kPid, &frame, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);
  }
  num_frames = kNumFrames;
  pm_status =
      pm_client.GetFrameData(kPid, false, &num_frames, frame_data.get());
  EXPECT_TRUE(num_frames == kNumFrames - 1);
  EXPECT_TRUE(num_frames == (uint32_t)frame_gen.GetNumFrames());
  EXPECT_TRUE(pm_status == PM_STATUS::PM_STATUS_SUCCESS);
  // Compare all recorded frames
  for (uint32_t i = 0; i <= ((uint32_t)frame_gen.GetNumFrames() - 1); i++) {
    EXPECT_TRUE(CompareFrameData(
        frame_data[i], frame_gen.GetPmFrameData(i, gpu_telemetry_cap_bits,
                                                cpu_telemetry_cap_bits)));
  }
}