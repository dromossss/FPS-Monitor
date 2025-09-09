// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "LoggedFrameDataState.h"
#include <format>
#include <Core/source/infra/log/Logging.h>
#include <Core/source/infra/util/Assert.h>
#include <Core/source/infra/util/Util.h>
#include <Core/source/infra/util/Util.h>
#include <ranges>
#include <CliCore/source/cons/ConsoleWaitControl.h>

namespace rn = std::ranges;
namespace vi = rn::views;

namespace p2c::cli::pmon::stream
{
    LoggedFrameDataState::LoggedFrameDataState(std::string filePath, cons::ConsoleWaitControl* pWaitControl)
        :
        cache_(initialCacheSize_),
        pWaitControl_{ pWaitControl }
    {
        if (auto sta = pmStartStreamEtl(filePath.c_str()); sta != PM_STATUS::PM_STATUS_SUCCESS)
        {
			p2clog.note(std::format(L"failed to open etl log file {} with error",
                infra::util::ToWide(filePath))).code(sta).commit();
		}
    }

    std::vector<FrameDataStream::Struct> LoggedFrameDataState::Pull(double timestamp, uint32_t pid)
    {
        if (!cacheTimestamp_ || cacheTimestamp_ != timestamp)
        {
            // in: max writeable buffer size, out: actual num frames written
            auto frameCountInOut = (uint32_t)initialCacheSize_;
            // cache buffer might have shrunk down real small, reset to initial size
            cache_.resize(initialCacheSize_);
            // where in vector to begin writing
            uint32_t writePosition = 0;
            while (true) {
                if (auto sta = pmGetEtlFrameData(&frameCountInOut, cache_.data() + writePosition);
                    sta == PM_STATUS::PM_STATUS_NO_DATA) {
                    // "shrink to fit"
                    cache_.resize(size_t(writePosition));
                    break;
                }
                else if (sta == PM_STATUS::PM_STATUS_PROCESS_NOT_EXIST) {
                    pWaitControl_->NotifyEndOfLogEvent();
                    break;
                }
                else if (sta != PM_STATUS::PM_STATUS_SUCCESS) {
                    p2clog.warn(std::format(L"failed to get etl log frame data with error")).code(sta).commit();
                    cache_.clear();
                    break;
                }
                // case where there *might* be more data
                // if the api writes to all available elements
                // means there might have been exactly the needed number
                // of elements in the buffer, or that there are more remaining
                if (size_t(writePosition) + size_t(frameCountInOut) >= std::size(cache_)) {
                    // if we're full, we should have filled up to exact size of buffer
                    CORE_ASSERT(size_t(writePosition) + size_t(frameCountInOut) == std::size(cache_));
                    // next frames write start just after end of current cache
                    writePosition = (uint32_t)std::size(cache_);
                    // double size of cache to accomodate more entries
                    cache_.resize(cache_.size() * 2);
                    // available space is total size of cache minus size already written
                    frameCountInOut = (uint32_t)std::size(cache_) - writePosition;
                }
                else {
                    // "shrink to fit"
                    cache_.resize(size_t(writePosition) + size_t(frameCountInOut));
                    break;
                }
            }
            cacheTimestamp_ = timestamp;
            // process all frames for simulating process spawn events
            for (const auto& frame : cache_) {
                if (auto&&[i, fresh] = pids_.insert(frame.process_id); fresh) {
                    pWaitControl_->SimulateSpawnEvent({
                        .pid = frame.process_id,
                        .name = infra::util::ToWide(frame.application),
                    });
                }
            }
        }
        // if pid is 0, return all frames
        if (pid == 0) {
            return cache_;
        }
        // filter out frames not for this pid
        return cache_ |
            vi::filter([pid](const auto& frame){ return frame.process_id == pid; }) |
            rn::to<std::vector>();
    }
}