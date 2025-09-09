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
    echo usage: stop_etl_collection.cmd path_to_xperf_exe
    exit /b 1
:args_ok

net session >nul 2>&1
if errorlevel 1 (
    echo error: must run as administrator
    exit /b 1
)

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
