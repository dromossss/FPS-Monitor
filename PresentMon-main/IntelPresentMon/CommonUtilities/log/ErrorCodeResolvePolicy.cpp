#include "ErrorCodeResolvePolicy.h"
#include "IErrorCodeResolver.h"
#include "Entry.h"

namespace pmon::util::log
{
	bool ErrorCodeResolvePolicy::TransformFilter(Entry& e)
	{
		if (e.errorCode_ && !e.errorCode_.IsResolved() && pResolver_) {
			e.errorCode_.Resolve(*pResolver_);
		}
		return true;
	}
	void ErrorCodeResolvePolicy::SetResolver(std::shared_ptr<IErrorCodeResolver> pResolver)
	{
		pResolver_ = std::move(pResolver);
	}
}