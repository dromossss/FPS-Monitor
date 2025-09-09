:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off
setlocal

:: Ensure we're running as administrator
net session >nul 2>&1
if not %errorlevel% == 0 (
    echo error: must run as administrator
    exit /b 1
)

:: Determine whether we're running on 32- or 64-bit
set platform=x64
set dllname=Intel-PresentMon.dll
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86"
if %errorlevel% EQU 0 (
    set platform=Win32
    set dllname=Intel-PresentMon32.dll
)

:: Get the dll directory from command line (or assume same dir as this script)
set dlldir=%~dp0.
if "%~1"=="" goto args_ok

set dlldir=%~1
if not "%~2"=="" goto usage
goto args_ok
:usage
    echo usage: install_provider.cmd DLL_DIR
    exit /b 1
:args_ok

call :setdllpath "%dlldir%\%dllname%"
if exist "%dllpath%" goto dllfound
    echo error: provider dll not found: %dllpath%
    goto usage
:dllfound

:: Check dependencies
if not exist "%SystemRoot%\System32\wevtutil.exe" (
    echo error: missing dependency: %SystemRoot%\System32\wevtutil.exe
    exit /b 1
)

if not exist "%~dp0Intel-PresentMon.man" (
    echo error: missing dependency: %~dp0Intel-PresentMon.man
    exit /b 1
)

:: Install manifest
"%SystemRoot%\System32\wevtutil.exe" im "%~dp0Intel-PresentMon.man" /rf:"%dllpath%" /mf:"%dllpath%"
if %errorlevel% NEQ 0 (
    echo error: failed to install manifest
    exit /b 1
)

:: Check that the install succeeded
"%SystemRoot%\System32\wevtutil.exe" gp Intel-PresentMon >NUL 2>&1
if %errorlevel% NEQ 0 (
    echo error: failed to install manifest
    exit /b 1
)

echo OK
exit /b 0


:setdllpath
    set dllpath=%~f1
    exit /b 0
