#pragma once
#include "../win/WinAPI.h"
#include <boost/asio.hpp>
#include <boost/asio/windows/object_handle.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <deque>

namespace pmon::util::pipe
{
	namespace as = boost::asio;

	class CoroMutex
	{
	public:
		using Timer = as::steady_timer;

		CoroMutex(as::io_context& ctx) : ctx_{ ctx } {}
		CoroMutex(const CoroMutex&) = delete;
		CoroMutex& operator=(const CoroMutex&) = delete;
		CoroMutex(CoroMutex&&) = default;
		CoroMutex& operator=(CoroMutex&&) = default;
		~CoroMutex() = default;

		as::awaitable<void> Lock();
		bool TryLock();
		void Unlock();
	private:
		as::io_context& ctx_;
		std::deque<Timer*> waiters_;
		int counter_ = 0;
		// used to check for the the same coro unlocking 2+ times
		bool holdoff_ = false;
	};

	class CoroLockGuard
	{
		friend as::awaitable<CoroLockGuard> CoroLock(CoroMutex& mtx);
		using CoroMutexType = CoroMutex;
	public:
		CoroLockGuard(const CoroLockGuard&) = delete;
		CoroLockGuard& operator=(const CoroLockGuard&) = delete;
		CoroLockGuard(CoroLockGuard&& other);
		CoroLockGuard& operator=(CoroLockGuard&& rhs);
		~CoroLockGuard();
	private:
		// functions
		CoroLockGuard(CoroMutexType& mtx) : pMtx_{ &mtx } {}
		// data
		CoroMutexType* pMtx_ = nullptr;
	};

	as::awaitable<CoroLockGuard> CoroLock(CoroMutex& mtx);
}