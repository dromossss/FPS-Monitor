#pragma once
#include "IChannelComponent.h"

namespace pmon::util::log
{
	// object that are meant to attach to channels for lifetime
	// management purposes only
	class IChannelObject : public IChannelComponent {};
}