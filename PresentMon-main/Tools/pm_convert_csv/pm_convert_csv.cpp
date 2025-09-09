// Copyright (C) 2017-2024 Intel Corporation
// SPDX-License-Identifier: MIT

#include <array>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_map>

namespace {

enum Columns {
    Application,
    ProcessID,
    SwapChainAddress,
    Runtime,
    SyncInterval,
    PresentFlags,
    Dropped,
    TimeInSeconds,
    msInPresentAPI,
    msBetweenPresents,
    AllowsTearing,
    PresentMode,
    msUntilRenderComplete,
    msUntilDisplayed,
    msBetweenDisplayChange,
    msUntilRenderStart,
    msGPUActive,
    msGPUVideoActive,
    msSinceInput,
    QPCTime,
    WasBatched,
    DwmNotified,
    NumColumns
};

struct Options {
    bool mTrackDisplay;
    bool mTrackGPU;
    bool mTrackGPUVideo;
    bool mTrackInput;
    bool mQpcTime;
};

struct PresentEvent {
    std::wstring Application;
    uint32_t     ProcessID;
    uint64_t     SwapChainAddress;
    std::wstring Runtime;
    uint32_t     SyncInterval;
    uint32_t     PresentFlags;
    bool         Dropped;
    double       TimeInSeconds;
    double       msInPresentAPI;
    bool         AllowsTearing;
    std::wstring PresentMode;
    double       msUntilRenderComplete;
    double       msUntilDisplayed;
    double       msBetweenDisplayChange;
    double       msUntilRenderStart;
    double       msGPUActive;
    double       msGPUVideoActive;
    double       msSinceInput;
    uint64_t     QPCTime;
};

struct SwapChainData {
    std::vector<PresentEvent> mPendingPresents;
    double mNextCPUFrameTime;
    bool mNextCPUFrameTimeIsValid = false;
};

using SwapChains = std::unordered_map<uint32_t, std::unordered_map<uint64_t, SwapChainData> >;

void WriteCsvHeader(Options const& opts)
{
    printf("Application"
           ",ProcessID"
           ",SwapChainAddress"
           ",Runtime"
           ",SyncInterval"
           ",PresentFlags");
    if (opts.mTrackDisplay) {
        printf(",AllowsTearing"
               ",PresentMode");
    }
    if (opts.mQpcTime) {
        printf(",CPUFrameQPC");
    } else {
        printf(",CPUFrameTime");
    }
    printf(",CPUDuration"
           ",CPUFramePacingStall");
    if (opts.mTrackGPU) {
        printf(",GPULatency"
               ",GPUDuration"
               ",GPUBusy");
    }
    if (opts.mTrackGPUVideo) {
        printf(",VideoBusy");
    }
    if (opts.mTrackDisplay) {
        printf(",DisplayLatency"
               ",DisplayDuration");
    }
    if (opts.mTrackInput) {
        printf(",InputLatency");
    }
    printf("\n");
}

void ReportMetrics(Options const& opts, SwapChainData* chain, PresentEvent const& p, PresentEvent const* nextDisplayedPresent)
{
    static bool first = true;
    static double t0 = 0.0;
    static uint64_t q0 = 0;
    if (first) {
        first = false;
        t0 = 1000.0 * p.TimeInSeconds;
        q0 = p.QPCTime;
    }

    // PB = PresentStartTime
    // PE = PresentEndTime
    // D  = ScreenTime
    // F  = CPUFrameTime/CPUDuration
    // S  = CPUFramePacingStall
    // D  = DisplayDuration
    //
    // msBetweenDisplayChange:           |----------->|
    // msUntilDisplayed:                 |  |-------->|
    // msBetweenPresents:      |----------->|         |
    //                         |         |  |         |
    // Previous PresentEvent:  PB--PE----D  |         |
    // p:                          |        PB--PE----D
    // Next PresentEvent(s):       |        |   |   PB--PE
    //                             |        |   |     |     PB--PE
    // nextDisplayedPresent:       |        |   |     |             PB--PE----D
    //                             |        |   |     |                       |
    // CPUFrameTime/CPUDuration:   |------->|   |     |                       |
    // CPUFramePacingStall:                 |-->|     |                       |
    // DisplayLatency:             |----------------->|                       |
    // DisplayDuration:                               |---------------------->|

    bool displayed = p.Dropped == false &&
                     nextDisplayedPresent != nullptr;

    double metrics_mCPUFrameTime        = 0.0;
    uint64_t metrics_mCPUFrameQPC       = 0;
    double metrics_mCPUDuration         = 0.0;
    double metrics_mCPUFramePacingStall = p.msInPresentAPI;
    double metrics_mGPULatency          = 0.0;
    double metrics_mGPUDuration         = p.msUntilRenderComplete - p.msUntilRenderStart;
    double metrics_mGPUBusy             = p.msGPUActive;
    double metrics_mVideoBusy           = p.msGPUVideoActive;
    double metrics_mDisplayLatency      = 0.0;
    double metrics_mDisplayDuration     = 0.0;
    double metrics_mInputLatency        = 0.0;

    if (chain->mNextCPUFrameTimeIsValid) {
        metrics_mCPUFrameTime = chain->mNextCPUFrameTime;
        metrics_mCPUDuration  = p.TimeInSeconds * 1000.0 - metrics_mCPUFrameTime;
        metrics_mGPULatency   = std::max(0.0, p.TimeInSeconds * 1000.0 + p.msUntilRenderStart - metrics_mCPUFrameTime);

        if (displayed) {
            metrics_mDisplayLatency  = std::max(0.0, nextDisplayedPresent->TimeInSeconds * 1000.0 +
                                                     nextDisplayedPresent->msUntilDisplayed -
                                                     nextDisplayedPresent->msBetweenDisplayChange -
                                                     metrics_mCPUFrameTime);

            if (p.msSinceInput != 0.0) {
                metrics_mInputLatency = metrics_mDisplayLatency + p.msSinceInput;
            }
        }

        if (opts.mQpcTime) {
            metrics_mCPUFrameQPC = q0 + (uint64_t) ((metrics_mCPUFrameTime - t0) * (p.QPCTime - q0) / (1000.0 * p.TimeInSeconds - t0) + 0.5);
        }
    }

    if (displayed) {
        metrics_mDisplayDuration = nextDisplayedPresent->msBetweenDisplayChange;
    }

    if (p.msUntilRenderStart == 0.0 && p.msGPUActive == 0.0 && (!opts.mTrackGPUVideo || p.msGPUVideoActive == 0.0)) {
        metrics_mGPULatency  = 0.0;
        metrics_mGPUDuration = 0.0;
        metrics_mGPUBusy     = 0.0;
        metrics_mVideoBusy   = 0.0;
    }

    printf("%ls,%d,0x%016llX,%ls,%d,%d", p.Application.c_str(),
                                         p.ProcessID,
                                         p.SwapChainAddress,
                                         p.Runtime.c_str(),
                                         p.SyncInterval,
                                         p.PresentFlags);
    if (opts.mTrackDisplay) {
        printf(",%d,%ls", p.AllowsTearing ? 1 : 0,
                          p.PresentMode.c_str());
    }
    if (chain->mNextCPUFrameTimeIsValid) {
        if (opts.mQpcTime) {
            printf(",%llu", metrics_mCPUFrameQPC);
        } else {
            printf(",%.6lf", metrics_mCPUFrameTime);
        }
        printf(",%.6lf", metrics_mCPUDuration);
    } else {
        printf(",,");
    }
    printf(",%.6lf", metrics_mCPUFramePacingStall);
    if (opts.mTrackGPU) {
        if (chain->mNextCPUFrameTimeIsValid) {
            printf(",%.6lf", metrics_mGPULatency);
        } else {
            printf(",");
        }
        printf(",%.6lf,%.6lf", metrics_mGPUDuration,
                               metrics_mGPUBusy);
    }
    if (opts.mTrackGPUVideo) {
        printf(",%.6lf", metrics_mVideoBusy);
    }
    if (opts.mTrackDisplay) {
        if (chain->mNextCPUFrameTimeIsValid) {
            printf(",%.6lf", metrics_mDisplayLatency);
        } else {
            printf(",");
        }
        printf(",%.6lf", metrics_mDisplayDuration);
    }
    if (opts.mTrackInput) {
        if (chain->mNextCPUFrameTimeIsValid) {
            printf(",%.6lf", metrics_mInputLatency);
        } else {
            printf(",");
        }
    }
    printf("\n");

    chain->mNextCPUFrameTime        = p.TimeInSeconds * 1000.0 + p.msInPresentAPI;
    chain->mNextCPUFrameTimeIsValid = true;
}

void usage()
{
    fprintf(stderr,
        "Convert a PresentMon v1.x CSV file into v2.0 CSV file.\n"
        "usage: pm_convert_csv.exe path_to_input.csv\n");
}

}

int wmain(
    int argc,
    wchar_t** argv)
{
    if (argc != 2) {
        usage();
        return 1;
    }

    std::wifstream file(argv[1]);
    if (!file.is_open()) {
        fprintf(stderr, "error: failed to open input file: %ls\n", argv[1]);
        usage();
        return 2;
    }

    SwapChains swapChains;

    std::wstring line;
    if (std::getline(file, line)) {
        std::wstringstream ss(line);

        if (line.size() >= 3 && line[0] == 0xef && line[1] == 0xbb && line[2] == 0xbf) {
            ss.seekg(3);
        }

        uint32_t columnIndex[NumColumns];
        for (uint32_t i = 0; i < NumColumns; ++i) {
            columnIndex[i] = UINT32_MAX;
        }

        std::wstring word;
        for (uint32_t i = 0; std::getline(ss, word, L','); ++i) {
                 if (word == L"Application")           columnIndex[Application]            = i;
            else if (word == L"ProcessID")             columnIndex[ProcessID]              = i;
            else if (word == L"SwapChainAddress")      columnIndex[SwapChainAddress]       = i;
            else if (word == L"Runtime")               columnIndex[Runtime]                = i;
            else if (word == L"SyncInterval")          columnIndex[SyncInterval]           = i;
            else if (word == L"PresentFlags")          columnIndex[PresentFlags]           = i;
            else if (word == L"Dropped")               columnIndex[Dropped]                = i;
            else if (word == L"TimeInSeconds")         columnIndex[TimeInSeconds]          = i;
            else if (word == L"msInPresentAPI")        columnIndex[msInPresentAPI]         = i;
            else if (word == L"msBetweenPresents")     columnIndex[msBetweenPresents]      = i;
            else if (word == L"AllowsTearing")         columnIndex[AllowsTearing]          = i;
            else if (word == L"PresentMode")           columnIndex[PresentMode]            = i;
            else if (word == L"msUntilRenderComplete") columnIndex[msUntilRenderComplete]  = i;
            else if (word == L"msUntilDisplayed")      columnIndex[msUntilDisplayed]       = i;
            else if (word == L"msBetweenDisplayChange")columnIndex[msBetweenDisplayChange] = i;
            else if (word == L"msUntilRenderStart")    columnIndex[msUntilRenderStart]     = i;
            else if (word == L"msGPUActive")           columnIndex[msGPUActive]            = i;
            else if (word == L"msGPUVideoActive")      columnIndex[msGPUVideoActive]       = i;
            else if (word == L"msSinceInput")          columnIndex[msSinceInput]           = i;
            else if (word == L"QPCTime")               columnIndex[QPCTime]                = i;
            else if (word == L"WasBatched")            columnIndex[WasBatched]             = i;
            else if (word == L"DwmNotified")           columnIndex[DwmNotified]            = i;
            else {
                fprintf(stderr, "error: unrecognised column: %ls\n", word.c_str());
                return 3;
            }
        }

        if (columnIndex[Application]       == UINT32_MAX ||
            columnIndex[ProcessID]         == UINT32_MAX ||
            columnIndex[SwapChainAddress]  == UINT32_MAX ||
            columnIndex[Runtime]           == UINT32_MAX ||
            columnIndex[SyncInterval]      == UINT32_MAX ||
            columnIndex[PresentFlags]      == UINT32_MAX ||
            columnIndex[Dropped]           == UINT32_MAX ||
            columnIndex[TimeInSeconds]     == UINT32_MAX ||
            columnIndex[msInPresentAPI]    == UINT32_MAX ||
            columnIndex[msBetweenPresents] == UINT32_MAX) {
            fprintf(stderr, "error: missing expected column.\n");
            return 4;
        }

        Options opts;
        opts.mTrackDisplay  = columnIndex[AllowsTearing]          != UINT32_MAX &&
                              columnIndex[PresentMode]            != UINT32_MAX &&
                              columnIndex[msUntilRenderComplete]  != UINT32_MAX &&
                              columnIndex[msUntilDisplayed]       != UINT32_MAX &&
                              columnIndex[msBetweenDisplayChange] != UINT32_MAX;
        opts.mTrackGPU      = columnIndex[msUntilRenderStart]     != UINT32_MAX &&
                              columnIndex[msGPUActive]            != UINT32_MAX;
        opts.mTrackGPUVideo = columnIndex[msGPUVideoActive]       != UINT32_MAX;
        opts.mTrackInput    = columnIndex[msSinceInput]           != UINT32_MAX;
        opts.mQpcTime       = columnIndex[QPCTime]                != UINT32_MAX;

        std::wstring row[NumColumns];
        bool firstRow = true;
        while (std::getline(file, line)) {
            ss.str(line);
            ss.clear();
            for (size_t i = 0; std::getline(ss, word, L','); ++i) {
                row[i] = word;
            }

            PresentEvent p;
            p.Application                 =             row[columnIndex[Application]];
            p.ProcessID                   = std::stoul (row[columnIndex[ProcessID]]);
            p.SwapChainAddress            = std::stoull(row[columnIndex[SwapChainAddress]], nullptr, 16);
            p.Runtime                     =             row[columnIndex[Runtime]];
            p.SyncInterval                = std::stol  (row[columnIndex[SyncInterval]]);
            p.PresentFlags                = std::stoul (row[columnIndex[PresentFlags]]);
            p.Dropped                     =             row[columnIndex[Dropped]] == L"1";
            p.TimeInSeconds               = std::stod  (row[columnIndex[TimeInSeconds]]);
            p.msInPresentAPI              = std::stod  (row[columnIndex[msInPresentAPI]]);
            if (opts.mTrackDisplay) {
                p.AllowsTearing           =             row[columnIndex[AllowsTearing]] == L"1";
                p.PresentMode             =             row[columnIndex[PresentMode]];
                p.msUntilRenderComplete   = std::stod  (row[columnIndex[msUntilRenderComplete]]);
                p.msUntilDisplayed        = std::stod  (row[columnIndex[msUntilDisplayed]]);
                p.msBetweenDisplayChange  = std::stod  (row[columnIndex[msBetweenDisplayChange]]);
            }
            if (opts.mTrackGPU) {
                p.msUntilRenderStart      = std::stod  (row[columnIndex[msUntilRenderStart]]);
                p.msGPUActive             = std::stod  (row[columnIndex[msGPUActive]]);
            }
            if (opts.mTrackGPUVideo) {
                p.msGPUVideoActive        = std::stod  (row[columnIndex[msGPUVideoActive]]);
            }
            if (opts.mTrackInput) {
                p.msSinceInput            = std::stod  (row[columnIndex[msSinceInput]]);
            }

            if (opts.mQpcTime) {
                auto const& qpcTime = row[columnIndex[QPCTime]];
                if (qpcTime.find('.') == std::string::npos) {
                    p.QPCTime = std::stoull(qpcTime);
                } else {
                    opts.mQpcTime = false;
                }
            }

            if (firstRow) {
                firstRow = false;
                WriteCsvHeader(opts);
            }

            auto chain = &swapChains[p.ProcessID][p.SwapChainAddress];

            if (p.Dropped) {
                if (chain->mPendingPresents.empty()) {
                    ReportMetrics(opts, chain, p, nullptr);
                } else {
                    chain->mPendingPresents.push_back(p);
                }
            } else {
                for (auto const& pp : chain->mPendingPresents) {
                    ReportMetrics(opts, chain, pp, &p);
                }
                chain->mPendingPresents.clear();
                chain->mPendingPresents.push_back(p);
            }
        }
    }

    file.close();
    return 0;
}
