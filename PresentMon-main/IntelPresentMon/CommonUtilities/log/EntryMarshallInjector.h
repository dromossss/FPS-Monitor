#pragma once
#include <memory>
#include "../mt/Thread.h"
#include "IChannelObject.h"

namespace pmon::util::log
{
	class IEntryMarshallReceiver;
	class IEntrySink;

	class EntryMarshallInjector : public IChannelObject
	{
	public:
		EntryMarshallInjector(std::weak_ptr<IEntrySink> pTo_, std::shared_ptr<IEntryMarshallReceiver> pSink);
		~EntryMarshallInjector() override;
	private:
		std::shared_ptr<IEntryMarshallReceiver> pFrom_;
		std::weak_ptr<IEntrySink> pTo_;
		mt::Thread worker_;
	};
}

