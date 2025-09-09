#pragma once
#include "IEntryMarshallSender.h"
#include "IdentificationTable.h"
#include <string>
#include <memory>
#include <semaphore>
#include "../pipe/SecurityMode.h"

namespace pmon::util::log
{
	struct Entry;

	class NamedPipeMarshallSender : public IEntryMarshallSender, public IIdentificationSink
	{
	public:
		struct EmptyHeader {};
		NamedPipeMarshallSender(const std::string& pipeName, pipe::SecurityMode security = pipe::SecurityMode::None);
        ~NamedPipeMarshallSender();
        void Push(const Entry& entry) override;
		void AddThread(uint32_t tid, uint32_t pid, std::string name) override;
		void AddProcess(uint32_t pid, std::string name) override;
		bool WaitForConnection(std::chrono::duration<float> timeout = std::chrono::duration<float>{ 0.f }) noexcept override;
	private:
		std::shared_ptr<void> pNamedPipe_;
		std::binary_semaphore constructionSema_{ 0 };
	};
}
