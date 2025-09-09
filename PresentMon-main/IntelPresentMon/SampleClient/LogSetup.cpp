#include "../CommonUtilities/log/Log.h"
#include "../CommonUtilities/log/Channel.h"
#include "../CommonUtilities/log/MsvcDebugDriver.h"
#include "../CommonUtilities/log/BasicFileDriver.h"
#include "../CommonUtilities/log/TextFormatter.h"
#include "../CommonUtilities/log/SimpleFileStrategy.h"
#include "../CommonUtilities/log/LinePolicy.h"
#include "../CommonUtilities/log/ErrorCodeResolvePolicy.h"
#include "../CommonUtilities/log/ErrorCodeResolver.h"
#include "../CommonUtilities/win/HrErrorCodeProvider.h"
#include "../CommonUtilities/str/String.h"
#include "../PresentMonAPIWrapperCommon/PmErrorCodeProvider.h"
#include "../PresentMonAPI2/Internal.h"
#include "CliOptions.h"
#include "LogSetup.h"
#include <memory>


namespace pmon::util::log
{
	namespace
	{
		std::shared_ptr<IChannel> MakeChannel()
		{
			// channel (use custom deleter to ensure deletion in this module's heap)
			auto pChannel = std::shared_ptr<IChannel>{ new Channel{}, [](Channel* p) { delete p; } };
			// error resolver
			auto pErrorResolver = std::make_shared<ErrorCodeResolver>();
			pErrorResolver->AddProvider(std::make_unique<win::HrErrorCodeProvider>());
			pErrorResolver->AddProvider(std::make_unique<pmapi::PmErrorCodeProvider>());
			// error resolving policy
			auto pErrPolicy = std::make_shared<ErrorCodeResolvePolicy>();
			pErrPolicy->SetResolver(std::move(pErrorResolver));
			pChannel->AttachComponent(std::move(pErrPolicy));
			// make the formatter
			const auto pFormatter = std::make_shared<TextFormatter>();
			const auto pFileStrategy = std::make_shared<SimpleFileStrategy>("log.txt");
			// make and add the line-tracking policy
			pChannel->AttachComponent(std::make_shared<LinePolicy>());
			// construct and configure default logging channel
			pChannel->AttachComponent(std::make_shared<MsvcDebugDriver>(pFormatter));
			pChannel->AttachComponent(std::make_shared<BasicFileDriver>(pFormatter, pFileStrategy));
			return pChannel;
		}
	}

	// this is injected into to util::log namespace and hooks into that system
	std::shared_ptr<IChannel> GetDefaultChannel() noexcept
	{
		return GetDefaultChannelWithFactory(MakeChannel);
	}
}

namespace p2sam
{
	using namespace pmon::util;
	using namespace pmon::util::log;

	void ConfigureLogging() noexcept
	{
		try {
			// connect dll channel and id table to exe, get access to global settings in dll
			const auto getters = pmLinkLogging_(GetDefaultChannel(), []()
				-> IdentificationTable& { return IdentificationTable::Get_(); });
			// shortcut for command line
			const auto& opt = clio::Options::Get();
			// configure logging based on command line
			if (opt.logLevel) {
				GlobalPolicy::Get().SetLogLevel(*opt.logLevel);
				getters.getGlobalPolicy().SetLogLevel(*opt.logLevel);
			}
			if (opt.logTraceLevel) {
				GlobalPolicy::Get().SetTraceLevel(*opt.logTraceLevel);
				getters.getGlobalPolicy().SetTraceLevel(*opt.logTraceLevel);
			}
			if (opt.logTraceExceptions) {
				GlobalPolicy::Get().SetExceptionTrace(*opt.logTraceExceptions);
				GlobalPolicy::Get().SetSehTracing(*opt.logTraceExceptions);
				getters.getGlobalPolicy().SetExceptionTrace(*opt.logTraceExceptions);
				getters.getGlobalPolicy().SetSehTracing(*opt.logTraceExceptions);
			}
			if (opt.logDenyList) {
				pmquell(LineTable::IngestList(*opt.logDenyList, true))
				pmquell(getters.getLineTable().IngestList_(*opt.logDenyList, true))
			}
			else if (opt.logAllowList) {
				pmquell(LineTable::IngestList(*opt.logAllowList, false))
				pmquell(getters.getLineTable().IngestList_(*opt.logAllowList, false))
			}
		}
		catch (...) {}
	}
	LogChannelManager::LogChannelManager() noexcept
	{
		InstallSehTranslator();
		BootDefaultChannelEager();
	}
	LogChannelManager::~LogChannelManager()
	{
		pmFlushEntryPoint_();
		FlushEntryPoint();
	}
}