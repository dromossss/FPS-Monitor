#include "BuildId.h"
#include "generated/build_id.h"

#ifndef PM_BUILD_CONFIG_
#ifdef NDEBUG
#define PM_BUILD_CONFIG_ "rel"
#else
#define PM_BUILD_CONFIG_ "dbg"
#endif
#endif

namespace pmon::bid
{
	const char* BuildIdShortHash() noexcept
	{
		return PM_BID_GIT_HASH_SHORT;
	}
	const char* BuildIdLongHash() noexcept
	{
		return PM_BID_GIT_HASH;
	}
	const char* BuildIdTimestamp() noexcept
	{
		return PM_BID_TIME;
	}
	const char* BuildIdUid() noexcept
	{
		return PM_BID_UID;
	}
	const char* BuildIdConfig() noexcept
	{
		return PM_BUILD_CONFIG_;
	}
	bool BuildIdDirtyFlag() noexcept
	{
		return PM_BID_DIRTY;
	}
}