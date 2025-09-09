#pragma once
#include "IChannelComponent.h"

namespace pmon::util::log
{
	struct Entry;

	class IPolicy : public IChannelComponent
	{
	public:
		virtual bool TransformFilter(Entry&) = 0;
	};
}
