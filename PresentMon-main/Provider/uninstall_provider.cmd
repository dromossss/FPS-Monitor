:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off

:: Ensure we're running as administrator
net session >nul 2>&1
if not %errorlevel% == 0 (
    echo error: must run as administrator
    exit /b 1
)

:: Check dependencies
if not exist "%SystemRoot%\System32\wevtutil.exe" (
    echo error: missing dependency: %SystemRoot%\System32\wevtutil.exe
    exit /b 1
)

if not exist "%~dp0Intel-PresentMon.man" (
    echo error: missing dependency: %~dp0Intel-PresentMon.man
    exit /b 1
)

:: Uninstall manifest
"%SystemRoot%\System32\wevtutil.exe" um "%~dp0Intel-PresentMon.man"
if %errorlevel% NEQ 0 (
    echo error: uninstall failed
    exit /b 1
)

:: Check that the uninstall succeeded
"%SystemRoot%\System32\wevtutil.exe" gp Intel-PresentMon >NUL 2>&1
if %errorlevel% EQU 0 (
    echo error: uninstall failed; ensure you are running as Administrator
    exit /b 1
)

echo OK
exit /b 0
