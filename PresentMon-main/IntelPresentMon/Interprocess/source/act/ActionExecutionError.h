#pragma once
#include "../PmStatusError.h"


namespace pmon::ipc::act
{
	class ActionExecutionError : public PmStatusError {
	public: ActionExecutionError(PM_STATUS code) : PmStatusError{ code, "Error occurred while processing Action" } {}
	};
}