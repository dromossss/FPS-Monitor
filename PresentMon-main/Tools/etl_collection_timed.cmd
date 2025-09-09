:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off
setlocal
set xperf=xperf.exe

if "%~1" equ "" goto check_args
if "%~1" neq "" set xperf=%1
if "%~2" neq "" goto usage
:check_args
    if not exist %xperf% (
        echo error: dependency not found: xperf.exe
        goto usage
    )
    goto args_ok
:usage
    echo usage: start_etl_collection.cmd path_to_xperf_exe
    exit /b 1
:args_ok

net session >nul 2>&1
if errorlevel 1 (
    echo error: must run as administrator
    exit /b 1
)

timeout /t 10

:: ProviderGUID[:Flags[:Level]]
set     d3d9=783ACA0A-790E-4d7f-8451-AA850511C6B9:0xf:6
set     dxgi=ca11c036-0102-4a2d-a6ad-f03cfed5d3c9:0xf:6
set  dxgkrnl=802ec45a-1e99-4b83-9920-87c98277ba9d
set      dwm=9e9bba3c-2e38-40cb-99f4-9e8281425164:0xffff:6
set dwm_win7=8c9dd1ad-e6e5-4b07-b455-684a9d879900:0xffff:6
set   win32k=8c416c79-d49b-4f01-a467-e56d3aa8234c:0x8400000440c01000:4
set      pmp=ecaa4712-4644-442f-b94c-a32f6cf8a499

set providers=LOADER
set providers=%providers%+PROC_THREAD
set providers=%providers%+CSWITCH
set providers=%providers%+DISPATCHER
%xperf% -on %providers% -BufferSize 1024 -MinBuffers 30 -MaxBuffers 120 -f Kernel.etl
if %errorlevel% neq 0 goto error

set providers=%dxgkrnl%:0x900236:5
set providers=%providers%+%d3d9%
set providers=%providers%+%dxgi%
%xperf% -start CaptureState -on %providers% -BufferSize 1024 -MinBuffers 30 -MaxBuffers 120 -f CaptureState.etl
if %errorlevel% neq 0 goto error

set providers=%dxgkrnl%:0x90FFFF:5
set providers=%providers%+%dxgi%
%xperf% -capturestate CaptureState %providers%
if %errorlevel% neq 0 goto error

set providers=%dxgkrnl%:0x04000000:5
%xperf% -start SchedulingLog -on %providers% -BufferSize 1024 -MinBuffers 30 -MaxBuffers 120 -f SchedulingLog.etl
if %errorlevel% neq 0 goto error

%xperf% -capturestate SchedulingLog %providers%
if %errorlevel% neq 0 goto error

set providers=%dxgkrnl%:0x208041:5
set providers=%providers%+%dwm%
set providers=%providers%+%dwm_win7%
set providers=%providers%+%pmp%
set providers=%providers%+%win32k%
%xperf% -start NoCaptureState -on %providers% -BufferSize 1024 -MinBuffers 30 -MaxBuffers 120 -f NoCaptureState.etl
if %errorlevel% neq 0 goto error

echo STARTED...

timeout /t 7

set error=0

%xperf% -capturestate SchedulingLog 802ec45a-1e99-4b83-9920-87c98277ba9d:0x04000000:5
if %errorlevel% neq 0 set error=1

%xperf% -stop CaptureState NoCaptureState SchedulingLog >NUL
if %errorlevel% neq 0 set error=1

%xperf% -stop >NUL
if %errorlevel% neq 0 set error=1

if %error% neq 0 exit /b 1

echo STOPPED
echo MERGING...

%xperf% -merge Kernel.etl NoCaptureState.etl CaptureState.etl SchedulingLog.etl trace.etl -compress -suppresspii

if exist Kernel.etl         del Kernel.etl
if exist NoCaptureState.etl del NoCaptureState.etl
if exist CaptureState.etl   del CaptureState.etl
if exist SchedulingLog.etl  del SchedulingLog.etl

exit /b 0

:error
    call "%~dp0stop_etl_collection.cmd" %xperf%
    exit /b 1
