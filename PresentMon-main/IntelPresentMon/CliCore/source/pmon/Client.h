// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <optional>
#include <vector>
#include <memory>
#include <unordered_map>
#include "AdapterInfo.h"
// adapters corresponding to pmapi structs/functions
#include "FrameDataStream.h"

namespace p2c::cli::cons
{
	class ConsoleWaitControl;
}

namespace p2c::cli::pmon
{
	namespace stream
	{
		class LoggedFrameDataState;
	}

	class Client
	{
	public:
		// types
		using AdapterInfo = pmon::AdapterInfo;
		// functions
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
		virtual ~Client();
		std::shared_ptr<FrameDataStream> OpenStream(uint32_t pid);
		void SetGpuTelemetryPeriod(uint32_t period);
		uint32_t GetGpuTelemetryPeriod();
		std::vector<AdapterInfo> EnumerateAdapters() const;
		void SetAdapter(uint32_t id);
		std::optional<uint32_t> GetSelectedAdapter() const;
	protected:
		Client(uint32_t telemetrySampleRateMs);
		virtual std::shared_ptr<FrameDataStream> MakeStream(uint32_t pid) = 0;
	private:
		std::unordered_map<uint32_t, std::weak_ptr<FrameDataStream>> streams_;
		uint32_t telemetrySamplePeriod_ = 0;
		std::optional<uint32_t> selectedAdapter_;
	};

	class LiveClient : public Client
	{
	public:
		LiveClient(uint32_t telemetrySampleRateMs = 16);
	protected:
		std::shared_ptr<FrameDataStream> MakeStream(uint32_t pid) override;
	};

	class LoggedClient : public Client
	{
	public:
		LoggedClient(std::string filePath, cons::ConsoleWaitControl& waitControl, uint32_t telemetrySampleRateMs = 16);
	protected:
		std::shared_ptr<FrameDataStream> MakeStream(uint32_t pid) override;
	private:
		std::shared_ptr<stream::LoggedFrameDataState> pLoggedState_;
	};
}