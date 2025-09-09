@echo off
if not exist "%~1\PresentMon.sln" (
    echo error: could not find %~1\PresentMon.sln
    echo usage: artifactory-copy.bat present_mon_root_dir destination_dir
    exit /b 1
)

set destination=%~2\PresentMon
echo %destination%

echo Removing previous artifacts
rmdir %destination% /s /q

echo Creating Destination Directories
mkdir %destination%\include\PresentMon
mkdir %destination%\lib
mkdir %destination%\pdb
mkdir %destination%\bin

echo Copying PresentMon header files...
xcopy "%~1\IntelPresentMon\PresentMonAPI2\PresentMonAPI.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\BlobContainer.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\DynamicQuery.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\FrameQuery.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\PresentMonAPIWrapper.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\ProcessTracker.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\Session.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapper\StaticQuery.h" "%destination%\include\PresentMon" /y
xcopy "%~1\IntelPresentMon\PresentMonAPIWrapperCommon\Introspection.h" "%destination%\include\PresentMon" /y

echo Copying PresentMon library files...
xcopy "%~1\build\Release\PresentMonAPI2.lib" "%destination%\lib" /y
xcopy "%~1\build\obj\PresentMonAPIWrapper-x64-Release\PresentMonAPIWrapper.lib" "%destination%\lib" /y
xcopy "%~1\build\obj\PresentMonAPIWrapperCommon-x64-Release\PresentMonAPIWrapperCommon.lib" "%destination%\lib" /y

echo Copying PresentMon binary files
xcopy "%~1\build\Release\PresentMonAPI2.dll" "%destination%\bin" /y
xcopy "%~1\build\Release\PresentMonService.exe" "%destination%\bin" /y

echo Copying PresentMon pdb files
xcopy "%~1\build\Release\PresentMonAPI2.pdb" "%destination%\pdb" /y
xcopy "%~1\build\obj\PresentMonAPIWrapper-x64-Release\PresentMonAPIWrapper.pdb" "%destination%\pdb" /y
xcopy "%~1\build\obj\PresentMonAPIWrapperCommon-x64-Release\PresentMonAPIWrapperCommon.pdb" "%destination%\pdb" /y