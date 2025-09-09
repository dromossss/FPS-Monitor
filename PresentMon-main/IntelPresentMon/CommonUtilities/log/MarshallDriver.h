#pragma once
#include "IDriver.h"


namespace pmon::util::log
{
	class IEntryMarshallSender;

	class MarshallDriver : public IDriver
	{
	public:
		MarshallDriver(std::shared_ptr<IEntryMarshallSender> pMarshallSender);
		void Submit(const Entry&) override;
		void Flush() override;
	private:
		std::shared_ptr<IEntryMarshallSender> pMarshallSender_;
	};
}

