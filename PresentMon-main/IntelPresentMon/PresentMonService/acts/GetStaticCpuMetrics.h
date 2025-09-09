#pragma once
#include "../../Interprocess/source/act/ActionHelper.h"
#include <format>

#define ACT_NAME GetStaticCpuMetrics
#define ACT_EXEC_CTX ActionExecutionContext
#define ACT_NS ::pmon::svc::acts
#define ACT_TYPE AsyncActionBase_

namespace pmon::svc::acts
{
	using namespace ipc::act;

	class ACT_NAME : public ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>
	{
	public:
		static constexpr const char* Identifier = STRINGIFY(ACT_NAME);
		struct Params {};
		struct Response
		{
			std::string cpuName;
			double cpuPowerLimit;

			template<class A> void serialize(A& ar) {
				ar(cpuName, cpuPowerLimit);
			}
		};
	private:
		friend class ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>;
		static Response Execute_(const ACT_EXEC_CTX& ctx, SessionContext& stx, Params&& in)
		{
			const Response out{
				.cpuName = ctx.pPmon->GetCpuName(),
				.cpuPowerLimit = ctx.pPmon->GetCpuPowerLimit()
			};
			pmlog_dbg(std::format("static CPU metrics gotten for {} @ {}W", out.cpuName, out.cpuPowerLimit));
			return out;
		}
	};

#ifdef PM_ASYNC_ACTION_REGISTRATION_
	ACTION_REG();
#endif
}

ACTION_TRAITS_DEF();

#undef ACT_NAME
#undef ACT_EXEC_CTX
#undef ACT_NS
#undef ACT_TYPE