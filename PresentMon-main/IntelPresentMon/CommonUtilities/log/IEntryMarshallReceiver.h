#pragma once
#include <optional>

namespace pmon::util::log
{
	struct Entry;

	class IEntryMarshallReceiver
	{
	public:
		virtual ~IEntryMarshallReceiver() = default;
		virtual std::optional<Entry> Pop() = 0;
		// after calling this function the next time Pop() would block, it instead returns an empty optional
		// useful to signalling to a thread that is pumping this MarshallReceiver that it should exit
		virtual void SignalExit() = 0;
	};
}