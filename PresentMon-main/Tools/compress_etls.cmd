:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off
setlocal enabledelayedexpansion
set rootdir=%~1
set xperf="%ProgramFiles(x86)%\Windows Kits\10\Windows Performance Toolkit\xperf.exe"

if "%~1" equ "" goto usage
if "%~2" equ "" goto check_args
if "%~2" neq "" set xperf=%2
if "%~3" neq "" goto usage
:check_args
    if not exist "%rootdir%\." (
        echo error: invalid root dir: %rootdir%
        goto usage
    )
    if not exist %xperf% (
        echo error: dependency not found: xperf.exe
        goto usage
    )
    goto args_ok
:usage
    echo usage: compress_etls.cmd RootDirWithEtls [XPerfPath]
    exit /b 1
:args_ok

for /f "tokens=*" %%a in ('dir /s /b /a-d "%rootdir%\*.etl"') do call :compress "%%a"
exit /b 0

:compress
    call :compress_helper %1 "%~dpn1.compressed_etl"
    exit /b 0

:compress_helper
    set oldsize=%~z1
    %xperf% -merge %1 %2 -compress >NUL 2>&1
    if not exist %2 (
        echo error: failed to create compressed file: %~2
        exit /b 1
    )
    set newsize=%~z2
    if %newsize% geq %oldsize% (
        echo %1 skipped.
    ) else (
        echo %1 compressed %oldsize% -^> %newsize% bytes.
        copy /Y %1 "%~1.orig" >NUL
        copy /Y %2 %1         >NUL
    )
    del %2
    exit /b 0
