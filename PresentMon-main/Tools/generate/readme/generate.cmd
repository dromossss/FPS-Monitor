:: Copyright (C) 2017-2024 Intel Corporation
:: SPDX-License-Identifier: MIT
@echo off
setlocal
set src=Tools\generate\readme
set dst=README-ConsoleApplication.md

pushd "%~dp0..\..\.."

set pmversion=
for /f %%a in ('Tools\awk.exe -f %src%\version.awk PresentMon.props') do set pmversion=%%a
echo PresentMon version: %pmversion%

type %src%\ConsoleIntro.md>%dst%

if "%pmversion%"=="dev" goto skip_version
    echo.>>%dst%
    echo A binary of the console application is provided in the release, e.g.:>>%dst%
    echo [PresentMon-%pmversion%-x64.exe]^(https://github.com/GameTechDev/PresentMon/releases/download/v%pmversion%/PresentMon-%pmversion%-x64.exe^).>>%dst%
:skip_version

echo.>>%dst%
echo ## Command line options>>%dst%

Tools\awk.exe -f %src%\args.awk PresentMon\CommandLine.cpp>>%dst%

echo.>>%dst%
type %src%\CSVIntro.md>>%dst%

echo.>>%dst%
Tools\awk.exe -f %src%\metrics.awk IntelPresentMon\metrics.csv>>%dst%

type %src%\CSVOutro.md>>%dst%

popd
exit /b 0
