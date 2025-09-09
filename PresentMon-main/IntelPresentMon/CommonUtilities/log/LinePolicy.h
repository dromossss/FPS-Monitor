#pragma once
#include "IPolicy.h"

namespace pmon::util::log
{
	// this policy handles rate control and white/black listing
	class LinePolicy : public IPolicy
	{
	public:
		bool TransformFilter(Entry& e) override;
	};
}
