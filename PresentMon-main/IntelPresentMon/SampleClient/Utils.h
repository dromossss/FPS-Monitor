#pragma once

std::string TranslatePresentMode(PM_PRESENT_MODE present_mode) {
    switch (present_mode) {
    case PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_LEGACY_FLIP:
        return "Hardware: Legacy Flip";

    case PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_LEGACY_COPY_TO_FRONT_BUFFER:
        return "Hardware: Legacy Copy to front buffer";
    case PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_INDEPENDENT_FLIP:
        return "Hardware: Independent Flip";
    case PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_FLIP:
        return "Composed: Flip";
    case PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_COMPOSED_INDEPENDENT_FLIP:
        return "Hardware Composed: Independent Flip";
    case PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_COPY_WITH_GPU_GDI:
        return "Composed: Copy with GPU GDI";
    case PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_COPY_WITH_CPU_GDI:
        return "Composed: Copy with CPU GDI";
    default:
        return("Present Mode: Unknown");
    }
}

std::string TranslateDeviceVendor(PM_DEVICE_VENDOR deviceVendor) {
    switch (deviceVendor) {
    case PM_DEVICE_VENDOR_INTEL:
        return "PM_DEVICE_VENDOR_INTEL";
    case PM_DEVICE_VENDOR_NVIDIA:
        return "PM_DEVICE_VENDOR_NVIDIA";
    case PM_DEVICE_VENDOR_AMD:
        return "PM_DEVICE_VENDOR_AMD";
    case PM_DEVICE_VENDOR_UNKNOWN:
        return "PM_DEVICE_VENDOR_UNKNOWN";
    default:
        return "PM_DEVICE_VENDOR_UNKNOWN";
    }
}

std::string TranslateGraphicsRuntime(PM_GRAPHICS_RUNTIME graphicsRuntime) {
    switch (graphicsRuntime) {
    case PM_GRAPHICS_RUNTIME_UNKNOWN:
        return "UNKNOWN";
    case PM_GRAPHICS_RUNTIME_DXGI:
        return "DXGI";
    case PM_GRAPHICS_RUNTIME_D3D9:
        return "D3D9";
    default:
        return "UNKNOWN";
    }
}

bool CaseInsensitiveCompare(std::string str1, std::string str2) {
    std::for_each(str1.begin(), str1.end(), [](char& c)
        {
            c = std::tolower(static_cast<unsigned char>(c));
        });
    std::for_each(str2.begin(), str2.end(), [](char& c)
        {
            c = std::tolower(static_cast<unsigned char>(c));
        });
    if (str1.compare(str2) == 0)
        return true;
    return false;
}

void GetProcessInformation(std::optional<std::string>& processName, std::optional<unsigned int>& processId) {

    try
    {
        // Explicitly set nullopt in optional values
        processId = std::nullopt;
        processName = std::nullopt;

        auto& opt = clio::Options::Get();
        if (bool(opt.processName) ^ bool(opt.processId)) {
            HANDLE processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if (processes_snapshot == INVALID_HANDLE_VALUE) {
                processId = std::nullopt;
                processName = std::nullopt;
                return;
            }

            PROCESSENTRY32 process_info;
            process_info.dwSize = sizeof(process_info);

            if (!Process32First(processes_snapshot, &process_info)) {
                // Unable to retrieve the first process
                CloseHandle(processes_snapshot);
                processId = std::nullopt;
                processName = std::nullopt;
                return;
            }

            do {
                if ((bool(opt.processName) && CaseInsensitiveCompare(process_info.szExeFile, *opt.processName)) ||
                    (bool(opt.processId) && process_info.th32ProcessID == *opt.processId)) {
                    CloseHandle(processes_snapshot);
                    processId = process_info.th32ProcessID;
                    processName = process_info.szExeFile;
                    return;
                }
            } while (Process32Next(processes_snapshot, &process_info));

            CloseHandle(processes_snapshot);
        }
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return;
    }
    catch (...) {
        std::cout << "Unknown Error" << std::endl;
        return;
    }
}