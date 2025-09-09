#include "NamedPipeMarshallSender.h"
#include "NamedPipeMarshallReceiver.h"
#include <stdexcept>
#include <sstream>
#include <vector>
#include "EntryCereal.h"
#include <cereal/archives/binary.hpp>
#include "../log/Log.h"
#include "MarshallingProtocol.h"

namespace pmon::util::log
{
    NamedPipeMarshallReceiver::NamedPipeMarshallReceiver(const std::string& pipeName, class IdentificationTable* pTable)
        :
        pipeName_{ R"(\\.\pipe\)" + pipeName },
        pipe_{ pipe::DuplexPipe::Connect(pipeName_, ioctx_)},
        pIdTable_{ pTable },
        exitEvent_{ ioctx_, win::Event{}.Release() }
    {}

    NamedPipeMarshallReceiver::~NamedPipeMarshallReceiver()
    {
        pmquell(SignalExit());
    }

    std::optional<Entry> NamedPipeMarshallReceiver::Pop()
    {
        if (sealed_) {
            throw std::runtime_error{ "Pop called after pipe sealed due to disconnection/error/exit signal" };
        }

        try {
            // setup an event to interrupt read blocking
            bool exiting = false;
            exitEvent_.async_wait([&, this](const boost::system::error_code& ec) {
                exiting = true; ioctx_.stop();
            });
            // launch coroutine to keep receiving until an Entry is received
            auto fut = pipe::as::co_spawn(ioctx_, [this]() -> pipe::as::awaitable<Entry> {
                // loop here until we get an Entry packet or throw an error
                // other control packets are processed without returning
                while (true) {
                    co_await pipe_.ReadPacketConsumeHeader<NamedPipeMarshallSender::EmptyHeader>();
                    const auto packet = pipe_.ConsumePacketPayload<MarshallPacket>();
                    if (auto pBulk = std::get_if<IdentificationTable::Bulk>(&packet)) {
                        if (pIdTable_) {
                            for (auto& p : pBulk->processes) {
                                pIdTable_->AddProcess(p.pid, std::move(p.name));
                            }
                            for (auto& t : pBulk->threads) {
                                pIdTable_->AddThread(t.tid, t.pid, std::move(t.name));
                            }
                        }
                    }
                    else if (auto pEntry = std::get_if<Entry>(&packet)) {
                        ioctx_.stop();
                        co_return std::move(*pEntry);
                    }
                }
            }, pipe::as::use_future);
            // run the async operations
            ioctx_.run();
            // if we completed run() due to exit being signalled, end receiver operation
            if (!exiting) {
                // otherwise we got an Entry, restart the context and return the Entry
                // This Pop() function will be called again after Entry is processed
                ioctx_.restart();
                return fut.get();
            }
        }
        catch (const pipe::PipeBroken&) {
            pmlog_dbg("Server disconnected pipe");
        }
        catch (...) {
            pmlog_error(ReportException()); throw;
        }
        sealed_ = true;
        return {};
    }

    void NamedPipeMarshallReceiver::SignalExit()
    {
        if (!SetEvent(exitEvent_.native_handle())) {
            pmlog_error("Failed setting exit event for NamedPipeMarshallReceiver");
        }
    }
}