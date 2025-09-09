#include "ErrorCodeResolver.h"


namespace pmon::util::log
{
	void ErrorCodeResolver::AddProvider(std::unique_ptr<IErrorCodeProvider> pProvider)
	{
		std::lock_guard lk{ mtx_ };
		providerMap_[pProvider->GetTargetType()] = std::move(pProvider);
	}

	ErrorCodeResolver::Strings ErrorCodeResolver::Resolve(std::type_index typeIdx, const ErrorCode& c) const noexcept
	{
		try {
			std::shared_lock lk{ mtx_ };
			if (auto i = providerMap_.find(typeIdx); i != providerMap_.end()) {
				return i->second->Resolve(c);
			}
		}
		catch (...) {}
		return {};
	}
}