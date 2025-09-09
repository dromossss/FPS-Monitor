#pragma once

#include <vector>

#include "NonCopyable.h"

namespace GfxLayer
{
    // Memory pool used to handle unwrapped object storage (structs, COM pointers, etc...)
    using Buffer = std::vector<uint8_t>;
    class MemoryPool: public NonCopyable
    {
    public:
        MemoryPool();
        ~MemoryPool();

        uint8_t*            NewBuffer(size_t len, const uint8_t* pData = nullptr);
        void                Reset();

    private:
        size_t              m_CurrentBufferIdx;
        std::vector<Buffer> m_Buffers;
    };
}