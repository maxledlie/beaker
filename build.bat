@echo off

REM Ensure we have an MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

if not exist build mkdir build

REM Compile all C files in src and lib. Link against OpenCL.
REM See https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_windows.md
REM for explainer of most command line parameters.
REM TODO: Don't hardcode paths to OpenCL SDK
cl ^
 /nologo ^
 /TC ^
 /W4 ^
 /DCL_TARGET_OPENCL_VERSION=100 ^
 /I..\vendor\OpenCL-SDK\install\include ^
 src\main.c ^
 /std:c11 ^
 /Fobuild\ ^
 /Fdbuild\main.pdb ^
 /Febuild\main.exe ^
 /Zi ^
 /Od ^
 /link /LIBPATH:..\vendor\OpenCL-SDK\install\lib OpenCL.lib

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)