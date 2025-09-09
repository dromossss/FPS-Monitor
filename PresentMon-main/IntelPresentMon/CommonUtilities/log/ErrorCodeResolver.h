#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <shared_mutex>
#include "IErrorCodeResolver.h"
#include "IErrorCodeProvider.h"

namespace pmon::util::log
{
	class ErrorCodeResolver : public IErrorCodeResolver
	{
	public:
		// thread safe
		void AddProvider(std::unique_ptr<IErrorCodeProvider>);
		// thread safe
		Strings Resolve(std::type_index typeIdx, const ErrorCode&) const noexcept override;
	private:
		mutable std::shared_mutex mtx_;
		std::unordered_map<std::type_index, std::unique_ptr<IErrorCodeProvider>> providerMap_;
	};
}