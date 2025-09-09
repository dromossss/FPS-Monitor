// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Client.h"
#include <Core/source/infra/log/Logging.h>
#include <PresentMonAPI/PresentMonAPI.h>
#include "stream/LiveFrameDataStream.h"
#include "stream/LoggedFrameDataStream.h"

namespace p2c::cli::pmon
{
	Client::Client(uint32_t telemetrySamplePeriodMs)
	{
		if (auto sta = pmInitialize(nullptr); sta != PM_STATUS::PM_STATUS_SUCCESS)
		{
			p2clog.note(L"could not init pmon").code(sta).commit();
		}
		SetGpuTelemetryPeriod(telemetrySamplePeriodMs);
	}
	Client::~Client()
	{
		if (auto sta = pmShutdown(); sta != PM_STATUS::PM_STATUS_SUCCESS) {
			p2clog.warn(L"could not shutdown pmon").code(sta).commit();
		}
	}
	std::shared_ptr<FrameDataStream> Client::OpenStream(uint32_t pid)
	{
		// if process exists in our map
		if (auto i = streams_.find(pid); i != streams_.end()) {
			// check if fresh
			if (auto pStream = i->second.lock()) {
				return pStream;
			}
			else {
				// if stale, replace stale entry with new proc stream and return
				pStream = MakeStream(pid);
				i->second = pStream;
				return pStream;
			}
		}
		else {
			auto pStream = MakeStream(pid);
			streams_[pid] = pStream;
			return pStream;
		}
	}
	void Client::SetGpuTelemetryPeriod(uint32_t period)
	{
		if (auto sta = pmSetGPUTelemetryPeriod(period); sta != PM_STATUS::PM_STATUS_SUCCESS)
		{
			p2clog.warn(std::format(L"could not set gpu telemetry sample period to {}", period)).code(sta).commit();
		}
		else
		{
			telemetrySamplePeriod_ = period;
		}
	}
	uint32_t Client::GetGpuTelemetryPeriod()
	{
		return telemetrySamplePeriod_;
	}
	std::vector<AdapterInfo> Client::EnumerateAdapters() const
	{
		uint32_t count = 0;
		if (auto sta = pmEnumerateAdapters(nullptr, &count); sta != PM_STATUS::PM_STATUS_SUCCESS)
		{
			p2clog.note(L"could not query adapter count").code(sta).commit();
		}
		std::vector<PM_ADAPTER_INFO> buffer{ size_t(count) };
		if (auto sta = pmEnumerateAdapters(buffer.data(), &count); sta != PM_STATUS::PM_STATUS_SUCCESS)
		{
			p2clog.note(L"could not enumerate adapters").code(sta).commit();
		}
		std::vector<AdapterInfo> infos;
		for (const auto& info : buffer)
		{
			const auto GetVendorName = [vendor = info.vendor] {
				using namespace std::string_literals;
				switch (vendor) {
				case PM_GPU_VENDOR::PM_GPU_VENDOR_AMD: return "AMD"s;
				case PM_GPU_VENDOR::PM_GPU_VENDOR_INTEL: return "Intel"s;
				case PM_GPU_VENDOR::PM_GPU_VENDOR_NVIDIA: return "Nvidia"s;
				default: return "Unknown"s;
				}
			};
			infos.push_back(AdapterInfo{
				.id = info.id,
				.vendor = GetVendorName(),
				.name = info.name,
			});
		}
		return infos;
	}
	void Client::SetAdapter(uint32_t id)
	{
		if (auto sta = pmSetActiveAdapter(id); sta != PM_STATUS::PM_STATUS_SUCCESS)
		{
			p2clog.note(L"could not set active adapter").code(sta).nox().commit();
		}
	}
	std::optional<uint32_t> Client::GetSelectedAdapter() const
	{
		return selectedAdapter_;
	}



	LiveClient::LiveClient(uint32_t telemetrySampleRateMs)
		:
		Client{ telemetrySampleRateMs }
	{}
	std::shared_ptr<FrameDataStream> LiveClient::MakeStream(uint32_t pid)
	{
		return std::make_shared<stream::LiveFrameDataStream>(pid);
	}



	LoggedClient::LoggedClient(std::string filePath, cons::ConsoleWaitControl& waitControl, uint32_t telemetrySampleRateMs)
		:
		Client{ telemetrySampleRateMs },
		pLoggedState_{ std::make_shared<stream::LoggedFrameDataState>(std::move(filePath), &waitControl) }
	{}
	std::shared_ptr<FrameDataStream> LoggedClient::MakeStream(uint32_t pid)
	{
		return std::make_shared<stream::LoggedFrameDataStream>(pid, pLoggedState_);
	}
}