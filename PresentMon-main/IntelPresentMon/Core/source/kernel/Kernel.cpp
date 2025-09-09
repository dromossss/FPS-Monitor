// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "Kernel.h"
#include <Core/source/win/WinAPI.h>
#include <Core/source/win/MessageBox.h>
#include <Core/source/infra/Logging.h>
#include <Core/source/infra/util/FolderResolver.h>
#include <random>
#include "OverlayContainer.h"
#include "TargetLostException.h"
#include <Core/source/win/ProcessMapBuilder.h>
#include <Core/source/win/com/WbemConnection.h>
#include <Core/source/cli/CliOptions.h>
#include <CommonUtilities/str/String.h>
#include <CommonUtilities/Exception.h>
#include <CommonUtilities/win/Utilities.h>
#include <boost/process/windows.hpp>


using namespace std::literals;
using namespace ::pmon::util;
namespace cwin = ::pmon::util::win;

namespace p2c::kern
{
    using str::ToWide;

    Kernel::Kernel(KernelHandler* pHandler)
        :
        pHandler{ pHandler },
        constructionSemaphore{ 0 },
        thread{ "kernel", &Kernel::ThreadProcedure_, this}
    {
        constructionSemaphore.acquire();
        HandleMarshalledException_();
    }

    Kernel::~Kernel()
    {
        {
            std::lock_guard lk{ mtx };
            dying = true;
        }
        cv.notify_one();
    }

    void Kernel::PushSpec(std::unique_ptr<OverlaySpec> pSpec)
    {
        HandleMarshalledException_();
        {
            std::lock_guard lk{ mtx };
            pPushedSpec = std::move(pSpec);
        }
        cv.notify_one();
    }

    void Kernel::UpdateInjection(bool enableInjection, std::optional<uint32_t> pid, bool enableBackground, 
        const gfx::Color& flashColor, const gfx::Color& backgroundColor, float width, float rightShift)
    {
        HandleMarshalledException_();
        std::lock_guard lk{ mtx };
        if (!enableInjection) {
            if (injectorProcess) {
                pmlog_dbg("terminating injector child");
                injectorProcess.reset();
            }
        }
        else {
            // routine to compare settings
            const auto SettingsMatch = [&]() {
                return
                    injectorProcess->enableBackground == enableBackground &&
                    injectorProcess->flashColor == flashColor &&
                    injectorProcess->backgroundColor == backgroundColor &&
                    injectorProcess->width == width &&
                    injectorProcess->rightShift == rightShift;
            };

            // cases where we have no pid
            if (!pid) {
                // no injector => nothing to do, so skip relaunching
                // yes injectr => if settings are the same skip relaunching
                if (!injectorProcess || SettingsMatch()) {
                    return;
                }
            }

            std::optional<std::string> nameCache;
            bool is32BitCache = false;
            uint32_t pidCache = 0;
            const auto LookupProcessInfo = [&](uint32_t pid) -> bool {
                try {
                    auto hProc = cwin::OpenProcess(pid);
                    is32BitCache = cwin::ProcessIs32Bit(hProc);
                    nameCache = cwin::GetExecutableModulePath(hProc).filename().string();
                    pidCache = pid;
                    return true;
                }
                catch (...) {
                    pmlog_warn("Failed target process lookup").pmwatch(pid);
                    return false;
                }
            };
            // case where we have pid AND injector, deep compare all the things
            if (pid && injectorProcess && SettingsMatch()) {
                // pid still matches means we don't even need to check the name
                if (*pid == injectorProcess->lastTrackedPid) {
                    return;
                }
                // lookup pid so we can compare name
                if (!LookupProcessInfo(*pid)) {
                    // pid changed but name was not in the cache
                    // we won't handle this case for now
                    pmlog_warn("pid not found in Process info cache, aborting injection").pmwatch(*pid);
                    injectorProcess.reset();
                    return;
                }
                // if name still matches, skip re-launch
                if (*nameCache == injectorProcess->trackedName) {
                    return;
                }
            }

            // if we made it this far, we need to (re-)launch the injector child process
            // if we have a new pid, make sure the name and bit-ness is populated
            if (pid && !nameCache) {
                if (!LookupProcessInfo(*pid)) {
                    pmlog_warn("pid not found in Process info cache, aborting injection").pmwatch(*pid);
                    injectorProcess.reset();
                    return;
                }
            }

            // if we make it here without a pid, then we must at least have injector
            // use its target info
            if (!pid) {
                is32BitCache = injectorProcess->is32Bit;
                nameCache = injectorProcess->trackedName;
                pidCache = injectorProcess->lastTrackedPid;
            }

            namespace bp = boost::process;
            const auto MakeColorString24 = [](const gfx::Color& c) {
                return std::format("{},{},{}",
                    int(c.r * 255.f), int(c.g * 255.f), int(c.b * 255.f));
            };
            const auto path = std::filesystem::current_path() / (is32BitCache ?
                    "FlashInjector-Win32.exe"s : "FlashInjector-x64.exe"s);
            std::vector<std::string> args;
            args.push_back("--exe-name"s); args.push_back(*nameCache);
            args.push_back("--bar-color"s); args.push_back(MakeColorString24(flashColor));
            args.push_back("--background-color"s); args.push_back(MakeColorString24(backgroundColor));
            args.push_back("--bar-size"s); args.push_back(std::to_string(width));
            args.push_back("--bar-right-shift"s); args.push_back(std::to_string(rightShift));
            if (enableBackground) {
                args.push_back("--render-background"s);
            }
            pmlog_dbg("launching injector child"s)
                .pmwatch(path.string())
                .pmwatch(*nameCache)
                .pmwatch(enableBackground)
                .pmwatch(MakeColorString24(flashColor))
                .pmwatch(MakeColorString24(backgroundColor))
                .pmwatch(width)
                .pmwatch(rightShift);
            injectorProcess.emplace(
                bp::child{ path.string(),
                    bp::args(std::move(args)),
                    bp::windows::hide
                }
            );
            injectorProcess->trackedName = std::move(*nameCache);
            injectorProcess->lastTrackedPid = pidCache;
            injectorProcess->is32Bit = is32BitCache;
            injectorProcess->enableBackground = enableBackground;
            injectorProcess->flashColor = flashColor;
            injectorProcess->backgroundColor = backgroundColor;
            injectorProcess->width = width;
            injectorProcess->rightShift = rightShift;
        }
    }

    void Kernel::ClearOverlay()
    {
        HandleMarshalledException_();
        {
            std::lock_guard lk{ mtx };
            if (pOverlayContainer) {
                clearRequested = true;
                pPushedSpec.reset();
            }
        }
        cv.notify_one();
    }

    void Kernel::SetAdapter(uint32_t id)
    {
        HandleMarshalledException_();
        std::lock_guard lk{ mtx };
        if (!pm) {
            pmlog_warn("presentmon not initialized");
            return;
        }
        pm->SetAdapter(id);
    }

    const pmapi::intro::Root& Kernel::GetIntrospectionRoot() const
    {
        HandleMarshalledException_();
        std::lock_guard g{ mtx };
        return pm->GetIntrospectionRoot();
    }

    std::vector<pmon::AdapterInfo> Kernel::EnumerateAdapters() const
    {
        HandleMarshalledException_();
        std::lock_guard lk{ mtx };
        if (!pm) {
            pmlog_warn("presentmon not initialized");
            return {};
        }
        try { return pm->EnumerateAdapters(); }
        catch (...) { 
            pmlog_warn("failed to enumerate adapters, returning empty set");
            return {};
        }
    }

    void Kernel::SetCapture(bool active)
    {
        HandleMarshalledException_();
        std::lock_guard lk{ mtx };
        pushedCaptureActive = active;
    }

    bool Kernel::IsIdle_() const
    {
        return !dying && !pPushedSpec && !pOverlayContainer;
    }

    std::unique_ptr<OverlaySpec> Kernel::PullSpec_()
    {
        std::lock_guard g{ mtx };
        return std::move(pPushedSpec);
    }

    void Kernel::HandleMarshalledException_() const
    {
        if (hasMarshalledException) {
            std::rethrow_exception(marshalledException);
        }
    }

    void Kernel::ThreadProcedure_()
    {
        try {
            // mutex that prevents frontend from accessing before pmon is connected
            std::unique_lock startLck{ mtx };

            // name this thread
            pmlog_info("== kernel thread starting ==");

            // command line options
            auto& opt = cli::Options::Get();

            // connect to wbem
            win::com::WbemConnection wbemConn;

            // connection names control from cli override / svc-as-child
            auto controlPipe = opt.controlPipe.AsOptional();
            // force optionals filled with default values if not specified when launching service as child
            if (opt.svcAsChild) {
                controlPipe = *opt.controlPipe;
            }

            // create the PresentMon object
            try { pm.emplace(controlPipe); }
            catch (...) {
                pHandler->OnPresentmonInitFailed();
                pmlog_error("Failed to init presentmon api").no_trace();
                throw;
            }

            startLck.unlock();
            constructionSemaphore.release();

            while (!dying)
            {
                try
                {
                    {
                        std::unique_lock u{ mtx };
                        cv.wait(u, [this] {return !IsIdle_(); });
                    }
                    if (pPushedSpec && !dying)
                    {
                        // spawn overlay (container)
                        ConfigurePresentMon_(*pPushedSpec);
                        pOverlayContainer = std::make_unique<OverlayContainer>(wbemConn, std::move(pPushedSpec), &*pm);
                    }
                    if (pOverlayContainer && !dying)
                    {
                        // blocks while overlay is active
                        RunOverlayLoop_();
                    }
                }
                // this catch section handles target lost and overlay crashes
                // control app will still work, can attempt to instance another overlay
                catch (const TargetLostException&)
                {
                    if (pOverlayContainer) {
                        pHandler->OnTargetLost(pOverlayContainer->GetProcess().pid);
                        pOverlayContainer.reset();
                        pPushedSpec.reset();
                    }
                    else {
                        pHandler->OnStalePidSelected();
                    }
                }
                catch (...) {
                    pHandler->OnOverlayDied();
                    pmlog_error("Overlay died w/ except. => " + ReportException().first).no_trace();
                    pOverlayContainer.reset();
                    pPushedSpec.reset();
                }
            }

            pm.reset();

            pmlog_info("== kernel thread exiting ==");
        }
        // this catch section handles failures to initialize kernel, or rare error that escape the main loop catch
        // possibility to marshall exceptions to js whenever an interface function is called (async rejection path)
        catch (...) {
            pmlog_error(ReportException()).no_trace();
            marshalledException = std::current_exception();
            hasMarshalledException.store(true);
        }
        constructionSemaphore.release();

        // make sure all the logging messages from kernel thread are processed
        if (auto chan = log::GetDefaultChannel()) {
            chan->Flush();
        }
    }

    void Kernel::RunOverlayLoop_()
    {
        // this loop runs while the overlay window is active
        while (pOverlayContainer)
        {
            // checking thread sync signals
            {
                std::lock_guard lk{ mtx };
                if (dying || clearRequested)
                {
                    pOverlayContainer->InitiateClose();
                    pPushedSpec.reset();
                    clearRequested = false;
                    inhibitTargetLostSignal = true;
                }
                else if (pPushedSpec)
                {
                    auto& curSpec = pOverlayContainer->GetSpec();
                    if (pPushedSpec->pid != curSpec.pid) {
                        pOverlayContainer->InitiateClose();
                    }
                    else if (pPushedSpec->independentKernelWindow != curSpec.independentKernelWindow ||
                        pPushedSpec->upscale != curSpec.upscale ||
                        (pPushedSpec->upscale && pPushedSpec->upscaleFactor != curSpec.upscaleFactor)) {
                        ConfigurePresentMon_(*pPushedSpec);
                        inhibitTargetLostSignal = true;
                        pOverlayContainer->RebootOverlay(std::move(pPushedSpec));
                    }
                    else {
                        ConfigurePresentMon_(*pPushedSpec);
                        pOverlayContainer->RebuildDocument(std::move(pPushedSpec));
                    }
                }
                else if (pushedCaptureActive)
                {
                    std::wstring path = infra::util::FolderResolver::Get()
                        .Resolve(infra::util::FolderResolver::Folder::Documents) + L"\\Captures\\";
                    pOverlayContainer->SetCaptureState(*pushedCaptureActive, std::move(path), pOverlayContainer->GetSpec().captureName);
                    pushedCaptureActive.reset();
                }
            }
            pOverlayContainer->CheckAndProcessFullscreenTransition();
            // process windows messages
            {
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                    {
                        if (!dying && !inhibitTargetLostSignal) {
                            pHandler->OnTargetLost(pOverlayContainer->GetProcess().pid);
                        }
                        inhibitTargetLostSignal = false;
                        pOverlayContainer.reset();
                        break;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
            // handle data and overlay
            if (pOverlayContainer)
            {
                pOverlayContainer->RunTick();
            }
        }
    }

    void Kernel::ConfigurePresentMon_(const OverlaySpec& newSpec)
    {
        if (newSpec.telemetrySamplingPeriodMs != pm->GetGpuTelemetryPeriod()) {
            pm->SetGpuTelemetryPeriod(newSpec.telemetrySamplingPeriodMs);
        }        
        if (const auto currentEtwPeriod = pm->GetEtwFlushPeriod();
            newSpec.manualEtwFlush != bool(currentEtwPeriod) ||
            (currentEtwPeriod && *currentEtwPeriod != newSpec.etwFlushPeriod)) {
            pm->SetEtwFlushPeriod(newSpec.manualEtwFlush ?
                std::optional{ uint32_t(newSpec.etwFlushPeriod) } : std::nullopt);
        }
    }
}
