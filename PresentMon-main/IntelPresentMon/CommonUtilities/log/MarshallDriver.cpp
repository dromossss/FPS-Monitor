#include "MarshallDriver.h"
#include "IEntryMarshallSender.h"

namespace pmon::util::log
{
	MarshallDriver::MarshallDriver(std::shared_ptr<IEntryMarshallSender> pMarshallSender)
		:
		pMarshallSender_{ std::move(pMarshallSender) }
	{}
	void MarshallDriver::Submit(const Entry& e)
	{
		pMarshallSender_->Push(e);
	}
	void MarshallDriver::Flush()
	{}
}