#pragma once
#include "../win/WinAPI.h"
#include "../win/Event.h"
#include <boost/asio.hpp>
#include <boost/asio/windows/object_handle.hpp>


namespace pmon::util::pipe
{
	namespace as = boost::asio;

	class ManualAsyncEvent
	{
		using Timer = as::steady_timer;
	public:
		ManualAsyncEvent(as::io_context& ctx)
			:
			timer_{ ctx, Timer::duration::max() }
		{}
		as::awaitable<void> AsyncWait()
		{
			auto ec = boost::system::error_code{};
			co_await timer_.async_wait(as::redirect_error(as::use_awaitable, ec));
		}
		void Signal()
		{
			timer_.cancel();
		}
	private:
		Timer timer_;
	};
}