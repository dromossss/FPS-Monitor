#include "gtest/gtest.h"
#include "..\Streamer\Streamer.h"
#include "..\Streamer\StreamClient.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <functional>
#include <tlhelp32.h>

#include "../CommonUtilities/log/GlogShim.h"

static const string kClientEXE = "SampleStreamerClient.exe";
static const string kMapFileName = "Global\\MyFileMappingObject";
static const string kSampleTestFile = "C:\\temp\\test.txt";
static const uint32_t kServerUpdateIntervalInMs = 10;
static const uint32_t kClientReadIntervalsInMs = 20;
static const string kSamplePresentMonFile = "testdata\\hitman3_presentmon_2.log";
static const string kSamplePresentMonFileSmall = "testdata\\hitman3_presentmon_1.log";
static const uint32_t kLoopCount = 20;
static const uint32_t kClientLoopCount = 20;
static const string sample_test_data = "HITMAN3.exe,1166288,0x000000004474CBE0,DXGI,1,0,1,0.04376850000000,0.39320000000000,16.52640000000000,0,Composed: Flip,32.33330000000000,0.00000000000000,0.00000000000000,785981.74005040002521";
static const uint32_t kNumFramesInBuf = 20;
static const uint64_t kNsmBufSize = 65535;
static const uint64_t kNsmBufSizeLarge = -1;


void ParsePresentMonCsvData(string line, PmNsmFrameData& data) {
	const std::regex delimiter(","); // whitespace
	std::sregex_token_iterator iter(line.begin(), line.end(), delimiter, -1);
	std::sregex_token_iterator end;

	// Skipping Application
	++iter;
	data.present_event.ProcessId = atoi(static_cast<string>(*iter).c_str());
	++iter;
	data.present_event.SwapChainAddress = std::stoull(static_cast<string>(*iter).c_str(), NULL, 16);
	// Skipping Runtime
	++iter;
	++iter;
	data.present_event.SyncInterval = atoi(static_cast<string>(*iter).c_str());
	++iter;
	//PresentFlags
	data.present_event.PresentFlags = atoi(static_cast<string>(*iter).c_str());
}


class StreamerULT : public ::testing::Test {
public:
    void SetUp() override {
    }

    void TearDown() override {
		LOG(INFO) << "Tear down ...";
	}

	Streamer streamer_;
	StreamClient client_;

	~StreamerULT() { 
	}

	void ServerRead(string file_name);
	bool reading_from_file_;
};


void StreamerULT::ServerRead(string file_name) {
	DWORD proc_id = GetCurrentProcessId();
	static const int kPathSize = 1024;

	string mapfile_name;
    GpuTelemetryBitset gpu_telemetry_cap_bits;
    CpuTelemetryBitset cpu_telemetry_cap_bits;
    gpu_telemetry_cap_bits.set();
    cpu_telemetry_cap_bits.set();
    
	streamer_.StartStreaming(proc_id, proc_id, mapfile_name);
	EXPECT_FALSE(mapfile_name.empty());
	
	std::ifstream test_read_file;
	test_read_file.open(file_name, std::fstream::in);


	if (test_read_file.is_open()) {
		LOG(INFO) << "\nFile successfully opened.";

		// skip the first line
		string line;
		getline(test_read_file, line);

		while (getline(test_read_file, line) && reading_from_file_) {
			LOG(INFO) << "\nWriting data...\n"<< line << std::endl;
			PmNsmFrameData data = { 0 };

			ParsePresentMonCsvData(line, data);

			streamer_.WriteFrameData(proc_id, &data,
                                                 gpu_telemetry_cap_bits,
                                                 cpu_telemetry_cap_bits);
			LOG(INFO) << "\nspin for " << kServerUpdateIntervalInMs << " ms";
			std::this_thread::sleep_for(std::chrono::milliseconds(kServerUpdateIntervalInMs));
		}
	}
	else {
		LOG(ERROR) << "\n Failed to open " << kSamplePresentMonFile;
	}

	test_read_file.close();
}

TEST_F(StreamerULT, StartStreaming) {
	DWORD proc_id = GetCurrentProcessId();
	auto status = pmStartStream(proc_id);
	EXPECT_EQ(status, PM_STATUS::PM_STATUS_SERVICE_NOT_INITIALIZED);
}

TEST_F(StreamerULT, StopStreaming) {
	DWORD proc_id = GetCurrentProcessId();

	string mapfile_name;
    streamer_.StartStreaming(proc_id, proc_id, mapfile_name);
	EXPECT_FALSE(mapfile_name.empty());

	// Stop with the same proc_id again
    streamer_.StopStreaming(proc_id);
    mapfile_name = streamer_.GetMapFileName(proc_id);
	// NSM should no longer exist
    EXPECT_TRUE(mapfile_name.empty());
}

TEST_F(StreamerULT, ClientOpenMappedFile) {
	std::thread serverthread(&StreamerULT::ServerRead, this, kSamplePresentMonFile);
	std::this_thread::sleep_for(std::chrono::milliseconds(kClientReadIntervalsInMs));

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	DWORD proc_id = GetCurrentProcessId();

	LOG(INFO) << "\nStreamer map file name for pid(" << proc_id << "):" << streamer_.GetMapFileName(proc_id);

	string cmdline_str = kClientEXE + " "+ streamer_.GetMapFileName(proc_id);

	LOG(INFO) << "\nCmdLine to start client process: " << cmdline_str;

	LPSTR cmdLine = const_cast<LPSTR>(cmdline_str.c_str());

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		cmdLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%lu).\n", GetLastError());
		return;
	}


	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	reading_from_file_ = false;
	serverthread.join();

	SUCCEED();
}


TEST_F(StreamerULT, ServerWriteData) {
	DWORD proc_id = GetCurrentProcessId();

	string mapfile_name;
    GpuTelemetryBitset gpu_telemetry_cap_bits;
    CpuTelemetryBitset cpu_telemetry_cap_bits;
    gpu_telemetry_cap_bits.set();
    cpu_telemetry_cap_bits.set();
        
	streamer_.StartStreaming(proc_id, proc_id, mapfile_name);
	EXPECT_FALSE(mapfile_name.empty());
	PmNsmFrameData data = {};

	ParsePresentMonCsvData(sample_test_data, data);

	streamer_.WriteFrameData(proc_id, &data, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);

	std::this_thread::sleep_for(std::chrono::milliseconds(kClientReadIntervalsInMs));

	StreamClient client(std::move(mapfile_name), false);
	
	PmNsmFrameData* client_read_data = nullptr;
	client_read_data = client.ReadLatestFrame();
	EXPECT_NE(client_read_data, nullptr);

	EXPECT_EQ(client_read_data->present_event.ProcessId, data.present_event.ProcessId);
	EXPECT_EQ(client_read_data->present_event.SwapChainAddress, data.present_event.SwapChainAddress);
	EXPECT_EQ(client_read_data->present_event.SyncInterval, data.present_event.SyncInterval);

	SUCCEED();
}

TEST(NamedSharedMemoryTest, CreateNamedSharedMemory) {
  DWORD proc_id = GetCurrentProcessId();
  EXPECT_NE(proc_id, 0);

  std::unique_ptr<Streamer> streamer = std::make_unique<Streamer>();
  streamer->CreateNamedSharedMemory(proc_id);
  EXPECT_NE(streamer->process_shared_mem_map_.find(proc_id),
            streamer->process_shared_mem_map_.end());
  string mapfilename = streamer->GetMapFileName(proc_id);
  EXPECT_EQ(mapfilename, kGlobalPrefix + std::to_string(proc_id));

  // Test standard etl case
  streamer->SetStreamMode(StreamMode::kOfflineEtl);
  streamer->CreateNamedSharedMemory(static_cast<uint32_t>(StreamPidOverride::kEtlPid));
  mapfilename = streamer->GetMapFileName(
      static_cast<uint32_t>(StreamPidOverride::kEtlPid));

  EXPECT_EQ(mapfilename, kGlobalPrefix + std::to_string(static_cast<uint32_t>(
                                             StreamPidOverride::kEtlPid)));
}

TEST(NamedSharedMemoryTestCustomSize, CreateNamedSharedMemory) {
  DWORD proc_id = GetCurrentProcessId();
  EXPECT_NE(proc_id, 0);

  std::unique_ptr<Streamer> streamer = std::make_unique<Streamer>();
  // Buf size 0
  streamer->CreateNamedSharedMemory(proc_id, 0);
  EXPECT_EQ(streamer->process_shared_mem_map_.find(proc_id),
            streamer->process_shared_mem_map_.end());

  // Normal buf size
  streamer->CreateNamedSharedMemory(proc_id, kNsmBufSize);
  EXPECT_NE(streamer->process_shared_mem_map_.find(proc_id),
            streamer->process_shared_mem_map_.end());
  string mapfilename = streamer->GetMapFileName(proc_id);
  EXPECT_EQ(mapfilename, kGlobalPrefix + std::to_string(proc_id));
  streamer->StopAllStreams();

  // Oversized buf size
  streamer->CreateNamedSharedMemory(proc_id, kNsmBufSizeLarge);
  EXPECT_EQ(streamer->process_shared_mem_map_.find(proc_id),
            streamer->process_shared_mem_map_.end());
   mapfilename = streamer->GetMapFileName(proc_id);
}

TEST_F(StreamerULT, ServerWriteDataOverflow) {
	// There are enough data to ensure write overflow 
	ServerRead(kSamplePresentMonFileSmall);
	DWORD proc_id = GetCurrentProcessId();

	string mapfile_name = streamer_.GetMapFileName(proc_id);
	EXPECT_FALSE(mapfile_name.empty());
	PmNsmFrameData data = {};
    GpuTelemetryBitset gpu_telemetry_cap_bits;
    CpuTelemetryBitset cpu_telemetry_cap_bits;
    gpu_telemetry_cap_bits.set();
    cpu_telemetry_cap_bits.set();

	ParsePresentMonCsvData(sample_test_data, data);

	streamer_.WriteFrameData(proc_id, &data, gpu_telemetry_cap_bits,
                                 cpu_telemetry_cap_bits);

	std::this_thread::sleep_for(std::chrono::milliseconds(kClientReadIntervalsInMs));

	StreamClient client(std::move(mapfile_name), false);

	PmNsmFrameData* client_read_data = nullptr;
	client_read_data = client.ReadLatestFrame();
	EXPECT_NE(client_read_data, nullptr);

	EXPECT_EQ(client_read_data->present_event.ProcessId, data.present_event.ProcessId);
	EXPECT_EQ(client_read_data->present_event.SwapChainAddress, data.present_event.SwapChainAddress);
	EXPECT_EQ(client_read_data->present_event.SyncInterval, data.present_event.SyncInterval);
}

TEST_F(StreamerULT, ReadBeforeWrite) {
	StreamClient client;

	PmNsmFrameData* client_read_data = nullptr;
	client_read_data = client.ReadLatestFrame();
	EXPECT_EQ(client_read_data, nullptr);
}