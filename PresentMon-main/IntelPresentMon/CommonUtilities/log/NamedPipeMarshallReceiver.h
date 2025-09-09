#pragma once
#include "IEntryMarshallReceiver.h"
#include <string>
#include "../win/WinAPI.h"
#include "../pipe/Pipe.h"
#include "../Exception.h"


namespace pmon::util::log
{
	class NamedPipeMarshallReceiver : public IEntryMarshallReceiver
	{
    public:
        NamedPipeMarshallReceiver(const std::string& pipeName, class IdentificationTable* pTable = nullptr);

        NamedPipeMarshallReceiver(const NamedPipeMarshallReceiver&) = delete;
        NamedPipeMarshallReceiver & operator=(const NamedPipeMarshallReceiver&) = delete;

        ~NamedPipeMarshallReceiver();
        std::optional<Entry> Pop() override;
        void SignalExit() override;
    private:
        bool sealed_ = false;
        std::string pipeName_;
        pipe::as::io_context ioctx_;
        pipe::DuplexPipe pipe_;
        boost::asio::windows::object_handle exitEvent_;
        class IdentificationTable* pIdTable_ = nullptr;
	};
}

