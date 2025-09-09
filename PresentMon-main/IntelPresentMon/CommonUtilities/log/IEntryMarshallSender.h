#pragma once
#include <optional>
#include <chrono>

namespace pmon::util::log
{
	struct Entry;

	class IEntryMarshallSender
	{
	public:
		virtual ~IEntryMarshallSender() = default;
		virtual void Push(const Entry& entry) = 0;
		virtual bool WaitForConnection(std::chrono::duration<float> timeout = std::chrono::duration<float>{ 0.f }) noexcept = 0;
	};
}