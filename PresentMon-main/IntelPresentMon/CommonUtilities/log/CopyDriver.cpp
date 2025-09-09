#include "CopyDriver.h"

namespace pmon::util::log
{
	CopyDriver::CopyDriver(std::shared_ptr<IChannel> pChannel)
		:
		pChannel_{ std::move(pChannel) }
	{}
	void CopyDriver::Submit(const Entry& e)
	{
		pChannel_->Submit(e);
	}
	void CopyDriver::Flush()
	{
		pChannel_->Flush();
	}
}