#include "CoroMutex.h"


namespace pmon::util::pipe
{
	as::awaitable<void> CoroMutex::Lock()
	{
		if (counter_++ == 0) {
			co_return;
		}

		auto ec = boost::system::error_code{};
		Timer timer{ ctx_ };
		timer.expires_after(Timer::duration::max());
		waiters_.push_back(&timer);
		co_await timer.async_wait(as::redirect_error(as::use_awaitable, ec));
		holdoff_ = false;
	}
	bool CoroMutex::TryLock()
	{
		if (counter_ == 0) {
			counter_++;
			return true;
		}
		return false;
	}
	void CoroMutex::Unlock()
	{
		// guard against unlocking twice from same coro or unlocked when there are no waiters
		if (counter_ == 0 || holdoff_) {
			return;
		}

		--counter_;

		if (waiters_.empty()) {
			return;
		}

		holdoff_ = true;
		waiters_.front()->cancel();
		waiters_.pop_front();
	}


	CoroLockGuard::CoroLockGuard(CoroLockGuard&& other)
		: pMtx_{ std::exchange(other.pMtx_, nullptr) }
	{}
	CoroLockGuard& CoroLockGuard::operator=(CoroLockGuard&& rhs)
	{
		pMtx_ = std::exchange(rhs.pMtx_, nullptr);
		return *this;
	}
	CoroLockGuard::~CoroLockGuard()
	{
		if (pMtx_) {
			pMtx_->Unlock();
		}
	}


	as::awaitable<CoroLockGuard> CoroLock(CoroMutex& mtx)
	{
		co_await mtx.Lock();
		co_return CoroLockGuard{ mtx };
	}
}