#pragma once
#include "IDriver.h"
#include "IChannel.h"


namespace pmon::util::log
{
	class CopyDriver : public IDriver
	{
	public:
		CopyDriver(std::shared_ptr<IChannel> pChannel);
		void Submit(const Entry&) override;
		void Flush() override;
	private:
		std::shared_ptr<IChannel> pChannel_;
	};
}
