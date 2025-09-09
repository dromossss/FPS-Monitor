#include <memory>

namespace pmon::util::log
{
	// this is injected into to util::log namespace and hooks into that system
	// not using logging yet in the tests so just return empty pointer
	std::shared_ptr<class IChannel> GetDefaultChannel() noexcept
	{
		return {};
	}
}