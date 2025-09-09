#pragma once
#include <Interprocess/source/act/SymmetricActionClient.h>
#include "cact/CefExecutionContext.h"

namespace p2c::client::util
{
    using namespace ::pmon::ipc;

    class CefClient : public act::SymmetricActionClient<cact::CefExecutionContext> {
    public: CefClient(std::string pipeName, cact::CefExecutionContext context = {});
    };
}