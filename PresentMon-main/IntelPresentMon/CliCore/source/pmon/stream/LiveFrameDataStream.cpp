// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "LiveFrameDataStream.h"
#include <format>
#include <Core/source/infra/log/Logging.h>
#include <Core/source/infra/util/Assert.h>

namespace p2c::cli::pmon::stream
{    
    LiveFrameDataStream::LiveFrameDataStream(uint32_t pid)
        :
        pid_(pid),
        cache_(initialCacheSize_)
    {
        if (auto sta = pmStartStream(pid_); sta != PM_STATUS::PM_STATUS_SUCCESS) {
            p2clog.note(std::format(L"could not start stream for pid {}", pid_)).code(sta).commit();
        }
        p2clog.info(std::format(L"started pmon stream for pid {}", pid_)).commit();
    }

    LiveFrameDataStream::~LiveFrameDataStream()
    {
        if (auto sta = pmStopStream(pid_); sta != PM_STATUS::PM_STATUS_SUCCESS) {
			p2clog.warn(std::format(L"could not stop stream for pid {}", pid_)).code(sta).commit();
		}
		p2clog.info(std::format(L"stopped pmon stream for pid {}", pid_)).commit();
	}

    std::span<const FrameDataStream::Struct> LiveFrameDataStream::Pull(double timestamp)
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
                if (auto sta = pmGetFrameData(pid_, &frameCountInOut, cache_.data() + writePosition);
                    sta == PM_STATUS::PM_STATUS_NO_DATA) {
                    // "shrink to fit"
                    cache_.resize(size_t(writePosition));
                    break;
                }
                else if (sta != PM_STATUS::PM_STATUS_SUCCESS) {
                    p2clog.warn(std::format(L"failed to get raw frame data with error [{}]", pid_)).code(sta).commit();
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
        }
        return cache_;
    }

    uint32_t LiveFrameDataStream::GetPid() const
    {
        return pid_;
    }
}