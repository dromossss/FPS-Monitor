#pragma once
#include "Event.h"
#include "WinAPI.h"
#include <utility>
#include <cassert>

namespace pmon::util::win
{
    class Overlapped
    {
    public:
        // types
        struct ConstructEmptyTag {};
        // functions
        Overlapped()
        {
            overlapped_.hEvent = event_;
        }
        Overlapped(ConstructEmptyTag) : event_{ Event::ConstructEmptyTag{} } {}
        Overlapped(Event event) : event_{ std::move(event) }
        {
            overlapped_.hEvent = event_;
        }
        ~Overlapped() = default;
        Overlapped(const Overlapped&) = delete;
        Overlapped& operator=(const Overlapped&) = delete;
        Overlapped(Overlapped&& other) noexcept = default;
        Overlapped& operator=(Overlapped&& other) noexcept = default;
        Event& GetEvent()
        {
            assert(event_);
            return event_;
        }
        operator OVERLAPPED*()
        {
            assert(event_);
            return &overlapped_;
        }
        operator bool() const noexcept
        {
            return (bool)event_;
        }
        void Clear()
        {
            event_.Clear();
        }
        void Reset()
        {
            event_.Reset();
            overlapped_ = { .hEvent = event_ };
        }
    private:
        Event event_;
        OVERLAPPED overlapped_{};
    };
}
