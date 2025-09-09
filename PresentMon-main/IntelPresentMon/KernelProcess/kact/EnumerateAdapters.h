#pragma once
#include "../../Interprocess/source/act/ActionHelper.h"
#include "KernelExecutionContext.h"
#include <format>
#include "../../Core/source/kernel/Kernel.h"
#include "../../Core/source/pmon/AdapterInfo.h"

#define ACT_NAME EnumerateAdapters
#define ACT_EXEC_CTX KernelExecutionContext
#define ACT_TYPE AsyncActionBase_
#define ACT_NS kproc::kact


namespace ACT_NS
{
	using namespace ::pmon::ipc::act;

	class ACT_NAME : public ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>
	{
	public:
		static constexpr const char* Identifier = STRINGIFY(ACT_NAME);
		struct Params
		{
			template<class A> void serialize(A& ar) {
			}
		};
		struct Response {
			std::vector<p2c::pmon::AdapterInfo> adapters;
			template<class A> void serialize(A& ar) {
				ar(adapters);
			}
		};
	private:
		friend class ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>;
		static Response Execute_(const ACT_EXEC_CTX& ctx, SessionContext& stx, Params&& in)
		{
			return { (*ctx.ppKernel)->EnumerateAdapters() };
		}
	};

	ACTION_REG();
}

namespace cereal
{
	template<class Archive>
	void serialize(Archive& archive, p2c::pmon::AdapterInfo& s)
	{
		archive(s.id, s.name, s.vendor);
	}
}

ACTION_TRAITS_DEF();

#undef ACT_NAME
#undef ACT_EXEC_CTX
#undef ACT_NS
#undef ACT_TYPE