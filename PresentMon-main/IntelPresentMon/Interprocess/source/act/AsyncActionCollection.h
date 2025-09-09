#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "../../../CommonUtilities/log/Log.h"
#include "AsyncAction.h"

namespace pmon::ipc::act
{
	template<class ExecutionContext>
	class AsyncActionCollection
	{
	public:
		AsyncActionCollection() = default;
		AsyncActionCollection(const AsyncActionCollection&) = delete;
		AsyncActionCollection& operator=(const AsyncActionCollection&) = delete;
		AsyncActionCollection(AsyncActionCollection&&) = delete;
		AsyncActionCollection& operator=(AsyncActionCollection&&) = delete;
		~AsyncActionCollection() = default;

		const AsyncAction<ExecutionContext>& Find(const std::string& key) const
		{
			return *actions_.at(key);
		}
		// Note: 1 collection allowed per context-process (module)
		static AsyncActionCollection& Get()
		{
			static AsyncActionCollection this_;
			return this_;
		}
		template<class T>
		void AddAction()
		{
			AddAction(std::make_unique<T>());
		}
		void AddAction(std::unique_ptr<AsyncAction<ExecutionContext>> pAction)
		{
			auto id = pAction->GetIdentifier();
			if (auto&& [i, inserted] = actions_.insert({ std::string{ id }, std::move(pAction) }); !inserted) {
				assert(false && "Duplicate key in AsyncActionCollection");
				pmlog_warn("Duplicate key for AsyncActionCollection").pmwatch(id);
			}
		}
	private:
		std::unordered_map<std::string, std::unique_ptr<AsyncAction<ExecutionContext>>> actions_;
	};

	template<class A, class E>
	struct AsyncActionRegistrator
	{
		AsyncActionRegistrator() {
			AsyncActionCollection<E>::Get().AddAction<A>();
		}
	};
}
