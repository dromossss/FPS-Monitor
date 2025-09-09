:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off
setlocal enabledelayedexpansion
set version=%1
set output_path=%2
set output_dir=%~dp2
set output_dir_short=%~dps2
set git_path=

set existing_branch=
set existing_commit=
if not exist %2 goto no_file
    for /f "tokens=5,6" %%a in (%2) do (
        set existing_branch=%%a
        set existing_commit=%%b
    )

    :: existing_branch should start with " and either existing_branch or
    :: existing_commit should end with ";.  This removes " and ; from them, but
    :: if existing_commit was empty this will convert it to "= so we patch that
    :: case after.
    set existing_branch=%existing_branch:"=%
    set existing_branch=%existing_branch:;=%
    set existing_commit=%existing_commit:"=%
    set existing_commit=%existing_commit:;=%
    if "%existing_commit:~-1%"=="=" set existing_commit=

    ::echo existing_branch=%existing_branch%
    ::echo existing_commit=%existing_commit%

    if "%version%"=="dev" goto compare_version
    if "%existing_branch%"=="%version%" if "%existing_commit%"=="" exit /b 0
    goto write_version

:no_file
    if not exist %output_dir_short%NUL (
        mkdir "%output_dir%"
        if not !errorlevel!==0 exit /b 1
    )

:compare_version

if not "%version%"=="dev" goto write_version
    set version=development branch

    for %%a in (git.exe) do set git_path=%%~$PATH:a
    if "%git_path%"=="" (
        echo warning: git not found in your PATH, built version will not include latest commit hash
        goto write_version
    )

    set branch=
    set commit=
    for /f %%a in ('"%git_path%" rev-parse --abbrev-ref HEAD') do set branch=%%a
    for /f %%a in ('"%git_path%" rev-parse HEAD') do set commit=%%a
    ::echo branch=%branch%
    ::echo commit=%commit%

    if "%branch%"=="" goto write_version
    if "%branch%"=="%existing_branch%" if "%commit%"=="%existing_commit%" exit /b 0

    set version=%branch% %commit%
    goto write_version

:write_version

echo updating %~2 to %version%
echo char const* PRESENT_MON_VERSION = "%version%";> %output_path%
if not exist %output_path% exit /b 1

exit /b 0
