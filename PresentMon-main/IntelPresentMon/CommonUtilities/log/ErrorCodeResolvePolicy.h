#pragma once
#include "IPolicy.h"
#include <memory>

namespace pmon::util::log
{
	class IErrorCodeResolver;

	class ErrorCodeResolvePolicy : public IPolicy
	{
	public:
		bool TransformFilter(Entry& e) override;
		void SetResolver(std::shared_ptr<IErrorCodeResolver>);
	private:
		std::shared_ptr<IErrorCodeResolver> pResolver_;
	};
}