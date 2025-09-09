#pragma once
#include "../../Interprocess/source/act/ActionHelper.h"
#include <format>

#define ACT_NAME StartTracking
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
		struct Params
		{
			uint32_t targetPid;

			template<class A> void serialize(A& ar) {
				ar(targetPid);
			}
		};
		struct Response
		{
			std::string nsmFileName;

			template<class A> void serialize(A& ar) {
				ar(nsmFileName);
			}
		};
	private:
		friend class ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>;
		static Response Execute_(const ACT_EXEC_CTX& ctx, SessionContext& stx, Params&& in)
		{
			std::string nsmFileName;
			if (auto sta = ctx.pPmon->StartStreaming(stx.remotePid, in.targetPid, nsmFileName); sta != PM_STATUS_SUCCESS) {
				pmlog_error("Start stream failed").code(sta);
				throw util::Except<ActionExecutionError>(sta);
			}
			stx.trackedPids.insert(in.targetPid);
			const Response out{ .nsmFileName = std::move(nsmFileName) };
			pmlog_info(std::format("StartTracking action from [{}] targeting [{}] assigned nsm [{}]",
				stx.remotePid, in.targetPid, out.nsmFileName));
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