#include "MemoryPool.h"
#include <iterator>

namespace GfxLayer
{
    MemoryPool::MemoryPool():
        m_CurrentBufferIdx(0)
    {}

    MemoryPool::~MemoryPool()
    {}

    uint8_t* MemoryPool::NewBuffer(size_t len, const uint8_t* pData)
    {
        Buffer* pNextBuffer = nullptr;
        auto    idx = m_CurrentBufferIdx++;

        if (idx < m_Buffers.size())
        {
            pNextBuffer = &m_Buffers[idx];
            if (pData)
            {
                pNextBuffer->clear();
                std::copy(pData, pData + len, std::back_inserter(*pNextBuffer));
            }
            else if (len > pNextBuffer->size()) 
            {
                pNextBuffer->resize(len);
            }
        }
        else
        {
            if (pData)
            {
                m_Buffers.emplace_back(pData, pData + len);
            }
            else
            {
                m_Buffers.emplace_back(len);
            }
            pNextBuffer = &m_Buffers[idx];
        }

        return pNextBuffer->data();
    }

    void MemoryPool::Reset()
    { 
        m_CurrentBufferIdx = 0;
    }
}