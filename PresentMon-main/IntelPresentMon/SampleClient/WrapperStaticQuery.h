#pragma once
#include "../PresentMonAPIWrapper/StaticQuery.h"
#include <locale>

int WrapperStaticQuerySample(std::unique_ptr<pmapi::Session>&& pSession)
{
    using namespace std::chrono_literals;

    try {
        std::optional<unsigned int> processId;
        std::optional<std::string> processName;

        GetProcessInformation(processName, processId);
        if (!processId.has_value()) {
            throw std::runtime_error{ 
                "Must set valid process name or process id:\n"
                "[--process-id id | --process-name]"
            };
        }

        auto& opt = clio::Options::Get();

        auto processTracker = pSession->TrackProcess(processId.value());

        // configure cout to (probably) add comma separators to numbers
        std::cout.imbue(std::locale{ "" });

        // struct to store results of static queries, used instead of direct stream
        // insertion to demonstrate automatic conversion functionality
        struct StaticInfo {
            std::string appName;
            uint64_t maxMemoryUint64;
            double maxMemoryDouble;
            std::string gpuVendorString;
            PM_DEVICE_VENDOR gpuVendorEnum;
            int gpuVendorInt;
        };

        for (const int i : std::views::iota(1)) {
            if (_kbhit()) break;

            std::cout << "Polling data iteration [" << i << "]...\n";

            // poll result can be directly assigned to compatible datatypes and will be intelligently
            // converted based on the static type of the left-hand side and introspection of metric
            const StaticInfo info{
                // string data is assignable to either std::string or wstring
                .appName = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_APPLICATION),
                // MEM_SIZE is natively uint64_t, assignment to original type always supported
                .maxMemoryUint64 = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_MEM_SIZE, 1),
                // numeric data is assignable to any numeric type,
                // blob will be interpreted with correct type then converted to destination type
                .maxMemoryDouble = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_MEM_SIZE, 1),
                // enumerations can be assigned to strings to perform auto-lookup of string metadata
                .gpuVendorString = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_VENDOR, 1),
                // or assigned to their actual enum type
                .gpuVendorEnum = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_VENDOR, 1),
                // or to an int to get the backing value
                .gpuVendorInt = pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_VENDOR, 1),
            };

            std::cout << "\nApp Name: " << info.appName
                << "\nMax Memory: " << info.maxMemoryUint64
                << "\nMax Memory as double: " << info.maxMemoryDouble
                << "\nGPU Vendor as string: " << info.gpuVendorString
                << "\nGPU Vendor as PM_DEVICE_VENDOR: " << (int)info.gpuVendorEnum
                << "\nGPU Vendor as int: " << info.gpuVendorInt
                // when the left-hand side conversion is ambiguous, you can explicitly force it with As<T>()
                << "\nGPU Name: " << pmapi::PollStatic(*pSession, processTracker, PM_METRIC_GPU_NAME, 1).As<std::string>();

            std::cout << "\n\nSleeping for 1 second...\n==============================" << std::endl;
            std::this_thread::sleep_for(1s);
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cout << "Unknown Error" << std::endl;
        return -1;
    }

    return 0;
}