#include "Log.h"
#include <memory>
#include "Channel.h"
#include "PanicLogger.h"
#include "IdentificationTable.h"
#include "LinePolicy.h"
#include "LineTable.h"
#include "../Exception.h"

namespace pmon::util::log
{
	namespace {
		// this is not atomic and might have a race condition
		// worst case scenario is that eager booting might be executed
		// more times than required, but benefit is faster checks in the
		// most frequent case (lock prefix instructions not used)
		bool dependenciesBooted_ = false;

		class ChannelManager_ {
		public:
			ChannelManager_(std::function<std::shared_ptr<IChannel>()> factory)
				:
				pChannel_{ factory() }
			{}
			std::shared_ptr<IChannel> Lock() const
			{
				// shared lock for reading
				std::shared_lock lk{ mtx_ };
				return pChannel_;
			}
			void Inject(std::shared_ptr<IChannel> pChannel)
			{
				// exclusive lock for writing
				std::lock_guard lk{ mtx_ };
				pChannel_ = std::move(pChannel);
			}
		private:
			mutable std::shared_mutex mtx_;
			std::shared_ptr<IChannel> pChannel_;
		};

		ChannelManager_& GetDefaultChannelContainer_(std::function<std::shared_ptr<IChannel>()> factory)
		{
			// make sure singleton dependencies are booted
			if (!dependenciesBooted_) {
				GlobalPolicy::Get().GetLogLevel();
				LineTable::TryLookup("", 0);
				IdentificationTable::LookupThread(0);
			}
			// @SINGLETON
			static ChannelManager_ channelManager{ std::move(factory) };
			return channelManager;
		}
	}
	std::shared_ptr<IChannel> GetDefaultChannelWithFactory(std::function<std::shared_ptr<IChannel>()> factory) noexcept
	{
		try {
			return GetDefaultChannelContainer_(std::move(factory)).Lock();
		}
		catch (...) {
			pmlog_panic_(ReportException());
		}
		return {};
	}
	void InjectDefaultChannel(std::shared_ptr<IChannel> pChannel) noexcept
	{
		try {
			GetDefaultChannelContainer_([] { return std::shared_ptr<IChannel>{}; })
				.Inject(std::move(pChannel));
		}
		catch (...) {
			pmlog_panic_(ReportException());
		}
	}
	void BootDefaultChannelEager() noexcept
	{
		try {
			std::thread{ [] {
				GetDefaultChannel();
			} }.detach();
		}
		catch (...) {
			pmlog_panic_(ReportException());
		}
	}
	void FlushEntryPoint() noexcept
	{
		GlobalPolicy::Get().SetResolveTraceInClientThread(true);
		if (auto pChan = GetDefaultChannel()) {
			pmquell(pChan->FlushEntryPointExit())
		}
	}
}