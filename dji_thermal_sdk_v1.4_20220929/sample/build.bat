:: Batch script which run cmake to build sample executable.
::
:: @Copyright (c) 2020-2023 DJI. All rights reserved.
::
:: Permission is hereby granted, free of charge, to any person obtaining a copy
:: of this software and associated documentation files (the "Software"), to deal
:: in the Software without restriction, including without limitation the rights
:: to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
:: copies of the Software, and to permit persons to whom the Software is
:: furnished to do so, subject to the following conditions:
::
:: The above copyright notice and this permission notice shall be included in
:: all copies or substantial portions of the Software.
::
:: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
:: IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
:: FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
:: AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
:: LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
:: OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
:: SOFTWARE.

@echo off
setlocal

goto MAIN
::-----------------------------------------------
:: "%~f2" get abs path of %~2.
::"%~fs2" get abs path with short names of %~2.
:setAbsPath
    setlocal
    set __absPath=%~f2
    endlocal && set %1=%__absPath%
    goto :eof
::-----------------------------------------------

:MAIN
call :setAbsPath PROJ_PATH ./
echo %PROJ_PATH%

set BUILD_TYPE_DEBUG=Debug
set BUILD_TYPE_RELEASE=Release

set BUILD=build\
set DIR_DEBUG=%BUILD_TYPE_DEBUG%
set DIR_RELEASE=%BUILD_TYPE_RELEASE%
set BUILD32=x86
set BUILD64=x64

set DIR_LIB=%PROJ_PATH%../tsdk-core/lib/windows

echo ****************************************
echo Clear build and output folders
echo ****************************************

if exist %PROJ_PATH%%BUILD% (
    echo Delete build folder
    del /f /s /q %PROJ_PATH%%BUILD%\*.*
    rd /q /s %PROJ_PATH%%BUILD%
)
if exist %PROJ_PATH%%DIR_DEBUG%_%BUILD32% (
    echo Delete output folder
    del /f /s /q %PROJ_PATH%%DIR_DEBUG%_%BUILD32%\*.*
    rd /q /s %PROJ_PATH%%DIR_DEBUG%_%BUILD32%
)
if exist %PROJ_PATH%%DIR_DEBUG%_%BUILD64% (
    echo Delete output folder
    del /f /s /q %PROJ_PATH%%DIR_DEBUG%_%BUILD64%\*.*
    rd /q /s %PROJ_PATH%%DIR_DEBUG%_%BUILD64%
)
if exist %PROJ_PATH%%DIR_RELEASE%_%BUILD32% (
    echo Delete output folder
    del /f /s /q %PROJ_PATH%%DIR_RELEASE%_%BUILD32%\*.*
    rd /q /s %PROJ_PATH%%DIR_RELEASE%_%BUILD32%
)
if exist %PROJ_PATH%%DIR_RELEASE%_%BUILD64% (
    echo Delete output folder
    del /f /s /q %PROJ_PATH%%DIR_RELEASE%_%BUILD64%\*.*
    rd /q /s %PROJ_PATH%%DIR_RELEASE%_%BUILD64%
)

echo ****************************************
echo Create build folder : %PROJ_PATH%%BUILD%
echo ****************************************

md %PROJ_PATH%%BUILD%

if exist %DIR_LIB%/debug_x86 (
    echo ****************************************
    echo Build Configuration %BUILD_TYPE_DEBUG% %BUILD32%
    echo ****************************************
    md %PROJ_PATH%%BUILD%%DIR_DEBUG%_%BUILD32%

    cd %PROJ_PATH%%BUILD%%DIR_DEBUG%_%BUILD32%
    cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE_DEBUG% ../../ -DTARGET_CPU=x86 -G "Visual Studio 14 2015"
    cmake --build . --target install --config %BUILD_TYPE_DEBUG%
)

if exist %DIR_LIB%/debug_x64 (
    echo ****************************************
    echo Build Configuration %BUILD_TYPE_DEBUG% %BUILD64%
    echo ****************************************
    md %PROJ_PATH%%BUILD%%DIR_DEBUG%_%BUILD64%

    cd %PROJ_PATH%%BUILD%%DIR_DEBUG%_%BUILD64%
    cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE_DEBUG% ../../ -DTARGET_CPU=x64 -G "Visual Studio 14 2015" -A x64
    cmake --build . --target install --config %BUILD_TYPE_DEBUG%
)

if exist %DIR_LIB%/release_x86 (
    echo ****************************************
    echo Build Configuration %BUILD_TYPE_RELEASE% %BUILD32%
    echo ****************************************
    md %PROJ_PATH%%BUILD%%DIR_RELEASE%_%BUILD32%

    cd %PROJ_PATH%%BUILD%%DIR_RELEASE%_%BUILD32%
    cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE_RELEASE% ../../ -DTARGET_CPU=x86 -G "Visual Studio 14 2015"
    cmake --build . --target install --config %BUILD_TYPE_RELEASE%
)

if exist %DIR_LIB%/release_x64 (
    echo ****************************************
    echo Build Configuration %BUILD_TYPE_RELEASE% %BUILD64%
    echo ****************************************
    md %PROJ_PATH%%BUILD%%DIR_RELEASE%_%BUILD64%

    cd %PROJ_PATH%%BUILD%%DIR_RELEASE%_%BUILD64%
    cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE_RELEASE% ../../ -DTARGET_CPU=x64 -G "Visual Studio 14 2015" -A x64
    cmake --build . --target install --config %BUILD_TYPE_RELEASE%
)

endlocal

pause
