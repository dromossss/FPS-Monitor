#include "EntryMarshallInjector.h"
#include "IEntryMarshallReceiver.h"
#include "IChannel.h"
#include "Entry.h"
#include "StackTrace.h"
#include "../Exception.h"
#include "PanicLogger.h"
#include "../str/String.h"

namespace pmon::util::log
{
	EntryMarshallInjector::EntryMarshallInjector(std::weak_ptr<IEntrySink> pTo, std::shared_ptr<IEntryMarshallReceiver> pSink)
		:
		pFrom_{ std::move(pSink) },
		pTo_{ std::move(pTo) }
	{
		worker_ = mt::Thread{ "log-prcv-inj", [this] {
			try {
				while (auto entry = pFrom_->Pop()) {
					if (auto p = pTo_.lock()) {
						p->Submit(std::move(*entry));
					}
				}
			}
			catch (...) {
				pmlog_panic_(ReportException());
			}
		}};
	}
	EntryMarshallInjector::~EntryMarshallInjector()
	{
		pmquell(pFrom_->SignalExit())
	}
}