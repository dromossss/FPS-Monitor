@echo off
rem -------------------------------------------------------
rem pull-cef.bat — Pull in CEF redist into consuming app
rem -------------------------------------------------------

if "%~1"=="" (
    echo usage: pull-cef.bat cef_dir
    exit /b 1
)

rem 1) Verify that the wrapper has been built
if not exist "%~1\build\cef.sln" (
    echo error: could not find %~1\build\cef.sln
    echo usage: pull-cef.bat cef_dir
    exit /b 1
)

rem 2) Define and clean the destination folder
set "destination=%~dp0..\Cef"
echo Destination: "%destination%"

if exist "%destination%" (
    echo Removing existing Cef folder...
    rmdir /S /Q "%destination%"
)

rem 3) Pull in binaries...
echo Pull in binaries...
xcopy /SY /exclude:%~dp0xcopy-ignore-bin.txt "%~1\Release" "%destination%\Bin\"

rem 4) Pull in libraries...
echo Pull in libraries...
xcopy /SY "%~1\build\libcef_dll_wrapper\Release" "%destination%\Lib\Release\"
xcopy /SY "%~1\Release\libcef.lib"                    "%destination%\Lib\Release\"
xcopy /SY "%~1\build\libcef_dll_wrapper\Debug"   "%destination%\Lib\Debug\"
xcopy /SY "%~1\Release\libcef.lib"                    "%destination%\Lib\Debug\"

rem 5) Pull in includes...
echo Pull in includes...
xcopy /SY "%~1\include" "%destination%\Include\include\"

rem 6) Pull in resources...
echo Pull in resources...
xcopy /SY "%~1\Resources" "%destination%\Resources\"

rem 7) Remove unused locales
echo Remove unused locales...
for %%i in ("%destination%\Resources\locales\*") do (
    if /I not "%%~nxi"=="en-US.pak" del "%%i"
)

echo Done.
