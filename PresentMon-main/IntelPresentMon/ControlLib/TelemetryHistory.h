// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "PresentMonPowerTelemetry.h"
#include "PowerTelemetryProvider.h"
#include <optional>

namespace pwr
{
    template<class T>
	class TelemetryHistory
	{
	public:
        class ConstIterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using reference = value_type&;
            using pointer = value_type*;
            using difference_type = ptrdiff_t;

            ConstIterator() = delete;
            ~ConstIterator() = default;
            ConstIterator(const TelemetryHistory* pContainer, size_t unwrapped_offset) noexcept : pContainer{ pContainer }, unwrapped_offset{ unwrapped_offset } {}
            ConstIterator(const ConstIterator& rhs) noexcept : pContainer{ rhs.pContainer }, unwrapped_offset{ rhs.unwrapped_offset } {}
            ConstIterator& operator=(const ConstIterator& rhs) noexcept
            {
                // Self-assignment check
                if (this != &rhs) {
                    pContainer = rhs.pContainer;
                    unwrapped_offset = rhs.unwrapped_offset;
                }
                return *this;
            }
            ConstIterator& operator+=(difference_type rhs) noexcept
            {
                unwrapped_offset += rhs;
                return *this;
            }
            ConstIterator& operator-=(difference_type rhs) noexcept
            {
                unwrapped_offset -= rhs;
                return *this;
            }
            const value_type& operator*() const noexcept
            {
                return pContainer->buffer[WrapIndex_(unwrapped_offset + pContainer->indexBegin)];
            }
            const value_type* operator->() const noexcept
            {
                return &**this;
            }
            const value_type& operator[](size_t rhs) const noexcept
            {
                return pContainer->buffer[WrapIndex_(unwrapped_offset + rhs + pContainer->indexBegin)];
            }
            
            ConstIterator& operator++() noexcept
            {
                ++unwrapped_offset;
                return *this;
            }
            ConstIterator& operator--() noexcept
            {
                --unwrapped_offset;
                return *this;
            }
            ConstIterator operator++(int) noexcept { ConstIterator tmp(*this); ++(*this); return tmp; }
            ConstIterator operator--(int) noexcept { ConstIterator tmp(*this); --(*this); return tmp; }
            difference_type operator-(const ConstIterator& rhs) const noexcept
            {
                return difference_type(unwrapped_offset) - difference_type(rhs.unwrapped_offset);
            }
            ConstIterator operator+(difference_type rhs) const noexcept
            {
                auto dup = *this;
                return dup += rhs;
            }
            ConstIterator operator-(difference_type rhs) const noexcept
            {
                auto dup = *this;
                return dup -= rhs;
            }

            bool operator==(const ConstIterator& rhs) const noexcept { return unwrapped_offset == rhs.unwrapped_offset; }
            bool operator!=(const ConstIterator& rhs) const noexcept { return unwrapped_offset != rhs.unwrapped_offset; }
            bool operator>(const ConstIterator& rhs) const noexcept { return unwrapped_offset > rhs.unwrapped_offset; }
            bool operator<(const ConstIterator& rhs) const noexcept { return unwrapped_offset < rhs.unwrapped_offset; }
            bool operator>=(const ConstIterator& rhs) const noexcept { return unwrapped_offset >= rhs.unwrapped_offset; }
            bool operator<=(const ConstIterator& rhs) const noexcept { return unwrapped_offset <= rhs.unwrapped_offset; }
        private:
            // functions
            size_t GetSize_() const noexcept
            {
                return pContainer->buffer.size();
            }
            // only wraps positive excursions
            size_t WrapIndex_(size_t unwrapped) const noexcept
            {
                return unwrapped % GetSize_();
            }

            // data
            const TelemetryHistory* pContainer;
            size_t unwrapped_offset;
        };

        TelemetryHistory(size_t size) noexcept;
        void Push(const T& info) noexcept;
		std::optional<T> GetNearest(uint64_t qpc) const noexcept;
        ConstIterator begin() const noexcept;
        ConstIterator end() const noexcept;
	private:
		std::vector<T> buffer;
        // indexBegin==buffer.size initial condition is necessary to indicate an empty container
        // (begin==end would normally indicate empty, but in a cyclic container this is ambiguous)
        size_t indexBegin;
        size_t indexEnd = 0;
	};

    template<class T>
    TelemetryHistory<T>::TelemetryHistory(size_t size) noexcept
        :
        indexBegin{ size }
    {
        buffer.resize(size);
    }

    template<class T>
    void TelemetryHistory<T>::Push(const T& info) noexcept
    {
        // indexBegin set to buffer size only in initial state 0 entries pushed
        if (indexBegin == buffer.size())
        {
            buffer[0] = info;
            indexBegin = 0;
            indexEnd = 1;
        }
        else if (indexBegin == indexEnd)
        {
            // full circle buffer, newest overwrites oldest
            buffer[indexEnd] = info;
            if (++indexEnd >= buffer.size())
            {
                indexEnd = 0;
            }
            indexBegin = indexEnd;
        }
        else
        {
            // buffer still filling
            buffer[indexEnd] = info;
            if (++indexEnd >= buffer.size())
            {
                indexEnd = 0;
            }
        }
    }

    template<class T>
    std::optional<T> TelemetryHistory<T>::GetNearest(uint64_t qpc) const noexcept
    {
        const auto begin = this->begin();
        const auto end = this->end();

        // return nothing if history empty
        if (begin == end) return {};

        const auto last = end - 1;

        // if outside the qpc history range return the closest values
        if (qpc > last->qpc) {
            return *last;
        } else if (qpc < begin->qpc) {
            return *begin;
        }

        // find lowest not less than query qpc
        const auto i = std::lower_bound(begin, end, qpc, [](const auto& a, auto b) {
            return a.qpc < b;
            });

        // if we're right on the money, no need to find closest among 2 neighboring
        if (i->qpc == qpc) return { *i };

        const auto distanceToLower = qpc - std::prev(i)->qpc;
        const auto distanceToUpper = i->qpc - qpc;

        return distanceToUpper <= distanceToLower ?
            std::optional<T>{ *i } :
            std::optional<T>{ *std::prev(i) };
    }

    template<class T>
    typename TelemetryHistory<T>::ConstIterator TelemetryHistory<T>::begin() const noexcept
    {
        return ConstIterator{ this, 0 };
    }

    template<class T>
    typename TelemetryHistory<T>::ConstIterator TelemetryHistory<T>::end() const noexcept
    {
        if (indexBegin == indexEnd) // begin/end indices locked mean container full
        {
            return ConstIterator{ this, buffer.size() };
        }
        else if (indexBegin == buffer.size()) // special case for empty container
        {
            return ConstIterator{ this, 0 };
        }
        else
        {
            return ConstIterator{ this, indexEnd }; // not yet cycling (not ful capacity)
        }
    }
}