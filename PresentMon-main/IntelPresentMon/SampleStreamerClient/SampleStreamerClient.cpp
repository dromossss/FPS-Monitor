// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
// SampleStreamerClient.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

#include "..\Streamer\StreamClient.h"

#include "../CommonUtilities/log/GlogShim.h"

uint32_t kClientLoopCount = 50;

int main(int argc, TCHAR* argv[])
{
	if (argc < 2) {
		LOG(ERROR) << "Mapfile name needed.";
	}

	std::string mapfile_name = argv[1];
	LOG(INFO) << "Mapfile name is " << mapfile_name;

	StreamClient client;

	try{
		client.Initialize(std::move(mapfile_name));
	} catch (const std::exception& e) {
		LOG(ERROR) << " a standard exception was caught, with message '"
                     << e.what() << "'\n";
        return 0;
	}

	uint32_t count = 0;

	while (count < kClientLoopCount) {

		PmNsmFrameData* data;

		data = client.ReadLatestFrame();
        if (data != nullptr) {
            try {
				LOG(INFO)
					<< "\nSampleStreamerClient read out ...\n"
					<< data->present_event.ProcessId << ", "
					<< data->present_event.SyncInterval << ", "
					<< data->present_event.PresentFlags << ", " << std::hex
					<< data->present_event.SwapChainAddress;
            } catch (const std::exception& e) {
				LOG(ERROR)
					<< " a standard exception was caught, with message '"
					<< e.what() << "'\n";
            }
        }
		count++;
	}
}