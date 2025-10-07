@echo off

REM Ensure we have an MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

if not exist build mkdir build

REM Compiler flags common to both CPU and OpenCL renderers
REM See https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_windows.md
set COMMON_FLAGS=^
 /nologo ^
 /TC ^
 /W4 ^
 /std:c11 ^
 /Zi ^
 /Od ^
 /Fobuild\ ^
 /Fdbuild\main.pdb ^
 /Iinclude ^
 /I..\vendor\OpenCL-SDK\install\include ^
 /DCL_TARGET_OPENCL_VERSION=100

set SOURCES=src\*.c lib\*.c
REM TODO: Don't hardcode paths to OpenCL SDK
set LIBPATH=/LIBPATH:..\vendor\OpenCL-SDK\install\lib

REM Build OpenCL renderer
cl %COMMON_FLAGS% ^
 %SOURCES% ^
 /Febuild\beaker_opencl.exe ^
 /link %LIBPATH% OpenCL.lib

if %ERRORLEVEL% neq 0 (
    echo OpenCL build failed!
    exit /b %ERRORLEVEL%
)

REM Build CPU renderer
cl %COMMON_FLAGS% ^
 %SOURCES% ^
 /Febuild\beaker_cpu.exe

if %ERRORLEVEL% neq 0 (
    echo CPU build failed!
    exit /b %ERRORLEVEL%
)

