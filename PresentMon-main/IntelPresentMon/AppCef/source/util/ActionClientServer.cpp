#include "ActionClientServer.h"
#include "../KernelProcess/kact/OpenSession.h"

namespace p2c::client::util
{
    CefClient::CefClient(std::string pipeName, cact::CefExecutionContext context)
        :
        SymmetricActionClient{ std::move(pipeName), std::move(context) }
    {
        auto res = DispatchSync(kproc::kact::OpenSession::Params{ GetCurrentProcessId() });
        EstablishSession_(res.kernelPid);
    }
}