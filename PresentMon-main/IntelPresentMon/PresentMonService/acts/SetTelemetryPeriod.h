#pragma once
#include "../../Interprocess/source/act/ActionHelper.h"
#include <format>
#include <ranges>

#define ACT_NAME SetTelemetryPeriod
#define ACT_EXEC_CTX ActionExecutionContext
#define ACT_NS ::pmon::svc::acts
#define ACT_TYPE AsyncActionBase_

namespace pmon::svc::acts
{
	using namespace ipc::act;
	namespace rn = std::ranges;
	namespace vi = rn::views;

	class ACT_NAME : public ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>
	{
	public:
		static constexpr const char* Identifier = STRINGIFY(ACT_NAME);
		struct Params
		{
			uint32_t telemetrySamplePeriodMs;

			template<class A> void serialize(A& ar) {
				ar(telemetrySamplePeriodMs);
			}
		};
		struct Response {};
	private:
		friend class ACT_TYPE<ACT_NAME, ACT_EXEC_CTX>;
		static Response Execute_(const ACT_EXEC_CTX& ctx, SessionContext& stx, Params&& in)
		{
			if (auto sta = ctx.pPmon->SetGpuTelemetryPeriod(in.telemetrySamplePeriodMs); sta != PM_STATUS_SUCCESS) {
				pmlog_error("Set telemetry period failed").code(sta);
				throw util::Except<ActionExecutionError>(sta);
			}
			stx.requestedTelemetryPeriodMs = in.telemetrySamplePeriodMs;
			pmlog_dbg(std::format("Setting telemetry sample period to {}ms", in.telemetrySamplePeriodMs));
			return {};
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
