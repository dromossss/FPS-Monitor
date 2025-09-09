@echo off

REM Initialize default values
set "BUILD_CONFIG=Release"
set "RUNTIME_LIB=Static"

REM Set defined variables
set BUILD_PATH=..\..\build
set OUT_PATH=%BUILD_PATH%\PresentMonAPIWrapper
set OBJ_PATH=%BUILD_PATH%\obj


REM Process command line arguments
:parse_args
if "%1"=="" goto end_parse_args
if "%1"=="--config" (
    goto snag_config
)
if "%1"=="--runtime" (
    goto snag_runtime
)
shift
goto parse_args
:snag_config
shift
set "BUILD_CONFIG=%1"
shift
goto parse_args
:snag_runtime
shift
set "RUNTIME_LIB=%1"
shift
goto parse_args
:end_parse_args


REM Find Visual Studio's path
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VS_PATH=%%i"
)

REM Check if Visual Studio path was found
if "%VS_PATH%"=="" (
    echo Visual Studio installation not found
    exit /b 1
)

REM Set paths to MSBuild and lib
set "MSBUILD_PATH=%VS_PATH%\MSBuild\Current\Bin\MSBuild.exe"

REM Assuming VS_PATH is already set to your Visual Studio installation directory
set "VC_TOOLS_PATH=%VS_PATH%\VC\Tools\MSVC"
REM Find the latest version of the VC tools directory
for /f "tokens=*" %%d in ('dir "%VC_TOOLS_PATH%" /b /ad-h /on') do set LATEST_VC_DIR=%%d
REM Now set the path to lib.exe using the latest VC tools version
set "LIB_PATH=%VC_TOOLS_PATH%\%LATEST_VC_DIR%\bin\Hostx64\x64\lib.exe"

echo Found lib.exe at: %LIB_PATH%

echo Found MSBuild at: %MSBUILD_PATH%
echo Found lib at: %LIB_PATH%


REM clean output directory to ensure binary compatibility of build artifacts (fixed debug info PDB warnings)
for /r "%BUILD_PATH%" %%f in (*) do if exist "%%f" del /q "%%f"
for /d %%d in ("%BUILD_PATH%\*") do rd /s /q "%%d"

REM build the wrapper
"%MSBUILD_PATH%" PresentMonAPIWrapper.vcxproj /p:Configuration="%BUILD_CONFIG%" /p:Platform="x64" /p:RuntimeOverride="%RUNTIME_LIB%"


if not exist "%OUT_PATH%" mkdir "%OUT_PATH%"

REM merge the lib files
"%LIB_PATH%" /OUT:"%OUT_PATH%\PresentMonAPIWrapper.lib" "%OBJ_PATH%\CommonUtilities-x64-%BUILD_CONFIG%\CommonUtilities.lib" "%OBJ_PATH%\PresentMonAPIWrapper-x64-%BUILD_CONFIG%\PresentMonAPIWrapper.lib" "%OBJ_PATH%\PresentMonAPIWrapperCommon-x64-%BUILD_CONFIG%\PresentMonAPIWrapperCommon.lib"

REM gather the headers from wrapper
xcopy ".\*.h" "%OUT_PATH%\PresentMonAPIWrapper\" /s /i /y

REM gather the headers from wrapper-common
xcopy "..\PresentMonAPIWrapperCommon\*.h" "%OUT_PATH%\PresentMonAPIWrapperCommon\" /s /i /y

REM gather the headers from common-utilities
xcopy "..\CommonUtilities\*.h" "%OUT_PATH%\CommonUtilities\" /s /i /y

REM gather the headers from the base C API
xcopy "..\PresentMonAPI2\*.h" "%OUT_PATH%\PresentMonAPI2\" /s /i /y

REM gather the headers from Interprocess
if not exist "%OUT_PATH%\Interprocess\source\metadata\" mkdir "%OUT_PATH%\Interprocess\source\metadata\"
set "IPC_SOURCE_DIR=..\Interprocess\source"
xcopy "%IPC_SOURCE_DIR%\IntrospectionDataTypeMapping.h" "%OUT_PATH%\Interprocess\source\" /Y
xcopy "%IPC_SOURCE_DIR%\metadata\EnumDataType.h" "%OUT_PATH%\Interprocess\source\metadata\" /Y
xcopy "%IPC_SOURCE_DIR%\metadata\MasterEnumList.h" "%OUT_PATH%\Interprocess\source\metadata\" /Y
