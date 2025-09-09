// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <string>

#include "../PresentMonUtils/StreamFormat.h"

static const uint64_t kBufSize = 65536 * 60;
static const std::string kGlobalPrefix = "Global\\NamedSharedMem_";

class NamedSharedMem {
 public:
  NamedSharedMem();
  NamedSharedMem(std::string mapfile_name, uint64_t buf_size,
      bool isPlayback,
      bool isPlaybackPaced,
      bool isPlaybackRetimed,
      bool isPlaybackBackpressured,
      bool isPlaybackResetOldest);
  ~NamedSharedMem();
  NamedSharedMem(const NamedSharedMem& t) = delete;
  NamedSharedMem& operator=(const NamedSharedMem& t) = delete;

  std::string GetMapFileName() { return mapfile_name_; }
  HANDLE GetMapFileHandle() { return mapfile_handle_; };
  // Get base offset of the frame data in shared memory. Normally this is
  // sizeof(NamedSharedMemoryHeader)
  uint32_t GetBaseOffset() { return data_offset_base_; };
  void* GetBuffer() { return buf_; };
  // Server only method to write frame data
  void WriteFrameData(PmNsmFrameData* data);
  // Server only method to write the telemetry bit caps to
  // the header
  void WriteTelemetryCapBits(
      std::bitset<static_cast<size_t>(GpuTelemetryCapBits::gpu_telemetry_count)>
          gpu_telemetry_cap_bits,
      std::bitset<static_cast<size_t>(CpuTelemetryCapBits::cpu_telemetry_count)>
          cpu_telemetry_cap_bits);

  // Client only method to pop already read frame data
  void DequeueFrameData();
  // Client only method to get the number of frames written by the
  // service
  uint64_t GetNumServiceWrittenFrames();
  // Client method to open a view into the shared mem
  void OpenSharedMemView(std::string mapfile_name);
  void NotifyProcessKilled();
  // TODO(jtseng2): header_ is client used only. Separate GetHeader() API from
  // server.
  const NamedSharedMemoryHeader* GetHeader() { return header_; };
  // indicates whether the ring buffer has available write space
  // only meaninful in a backpressured (typically replay) scenario
  bool IsFull();
  // indicates whether the ring buffer has any unconsumed frames
  // only meaninful in a backpressured (typically replay) scenario
  bool IsEmpty();
  // indicates whether every element in the ring buffer contains valid frame information
  // typically, this is only false until the buffer has wrapped once
  bool HasUninitializedFrames();
  bool IsNSMCreated() { return buf_created_; };

  // Helper frunctions for generating frame statistics
  // Cache first frame's qpc time as the start time
  void RecordFirstFrameTime(uint64_t start_qpc);
  // Cache last displayed frame id during WriteFrameData
  uint64_t RecordAndGetLastDisplayedQpc(uint64_t qpc);
  void IncrementRefcount() { refcount_++;};
  void DecrementRefcount() { refcount_--; };
  int GetRefCount() { return refcount_; };
  uint64_t GetBufSize() { return buf_size_; };

 private:
  // Server method to create a shared mem in buf_size bytes
  HRESULT CreateSharedMem(std::string mapfile_name, uint64_t buf_size);
  void OutputErrorLog(const char* error_string, DWORD last_error);
  std::string mapfile_name_;
  HANDLE mapfile_handle_;
  uint32_t data_offset_base_;
  NamedSharedMemoryHeader* header_;
  void* buf_;
  // allocation granularity obtained from GetSystemInfo. MapViewOfFile offset
  // must be multiple of alloc_granularity
  size_t alloc_granularity_;
  int refcount_;
  bool buf_created_;
  uint64_t buf_size_;
};