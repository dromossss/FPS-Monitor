// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <windows.h>
#include <thread>
#include <string>
#include <map>
#include "../PresentMonUtils/StreamFormat.h"
#include "../PresentMonUtils/LegacyAPIDefines.h"
#include "NamedSharedMemory.h"

class StreamClient {
 public:
  StreamClient();
  StreamClient(std::string mapfile_name, bool is_etl_stream_client);
  ~StreamClient();

  void Initialize(std::string mapfile_name);
  bool IsInitialized() { return initialized_; };
  // Read the latest frame from shared memory.
  PmNsmFrameData* ReadLatestFrame();
  PmNsmFrameData* ReadFrameByIdx(uint64_t frame_id, bool checked);
  // Dequeue a frame of data from shared mem and update the last_read_idx (just get pointer to NsmData)
  PM_STATUS ConsumePtrToNextNsmFrameData(const PmNsmFrameData** pNsmData,
                                         const PmNsmFrameData** pNextFrame,
                                         const PmNsmFrameData** pFrameDataOfNextDisplayed,
                                         const PmNsmFrameData** pFrameDataOfLastPresented,
                                         const PmNsmFrameData** pFrameDataOfLastAppPresented,
                                         const PmNsmFrameData** pFrameDataOfLastDisplayed,
                                         const PmNsmFrameData** pFrameDataOfLastAppDisplayed,
                                         const PmNsmFrameData** pFrameDataOfPreviousAppFrameOfLastAppDisplayed);
  // Return the last frame id that holds valid data
  uint64_t GetLatestFrameIndex();
  NamedSharedMem* GetNamedSharedMemView() { return shared_mem_view_.get(); }
  void CloseSharedMemView();
  LARGE_INTEGER GetQpcFrequency() { return qpcFrequency_; };
  void CopyFrameData(uint64_t start_qpc, const PmNsmFrameData* src_frame,
                     GpuTelemetryBitset gpu_telemetry_cap_bits,
                     CpuTelemetryBitset cpu_telemetry_cap_bits,
                     PM_FRAME_DATA* dst_frame);

  std::optional<std::bitset<
      static_cast<size_t>(GpuTelemetryCapBits::gpu_telemetry_count)>>
  GetGpuTelemetryCaps();
  std::optional<std::bitset<
      static_cast<size_t>(CpuTelemetryCapBits::cpu_telemetry_count)>>
  GetCpuTelemetryCaps();

 private:
  uint64_t CheckPendingReadFrames();

  // Functions to peek at the next and previous frames
  void PeekNextFrames(const PmNsmFrameData** pNextFrame,
                      const PmNsmFrameData** pNextDisplayedFrame);
  void PeekPreviousFrames(const PmNsmFrameData** pFrameDataOfLastPresented,
                          const PmNsmFrameData** pFrameDataOfLastAppPresented,
                          const PmNsmFrameData** pFrameDataOfLastDisplayed,
                          const PmNsmFrameData** pFrameDataOfLastAppDisplayed,
                          const PmNsmFrameData** pFrameDataOfPreviousAppFrameOfLastAppDisplayed);

  // Helper functions evaluater various frame types
  bool IsAppPresentedFrame(const PmNsmFrameData* frame) const;
  bool IsDisplayedFrame(const PmNsmFrameData* frame) const;
  bool IsAppDisplayedFrame(const PmNsmFrameData* frame) const;

  // Shared memory view that the client opened into based on mapfile name
  std::unique_ptr<NamedSharedMem> shared_mem_view_;
  // mapfile name the client has for named shared memory
  std::string mapfile_name_;
  // Last read offset from the shared_mem_view_
  bool initialized_;
  LARGE_INTEGER qpcFrequency_ = {};
  uint64_t next_dequeue_idx_;
  bool recording_frame_data_;
  uint64_t current_dequeue_frame_num_;
};