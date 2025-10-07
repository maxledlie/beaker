@echo off

REM Ensure we have an MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul

if not exist build mkdir build

REM Compile all C files in src and lib
cl src\*.c lib\*.c /Wall /std:c11 -Fo"build/" -Fe"build/main.exe" -I include
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo Running program...
.\build\main.exe
if %ERRORLEVEL% neq 0 (
    echo Program failed!
    exit /b %ERRORLEVEL%
)