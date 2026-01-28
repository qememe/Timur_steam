@echo off
setlocal enabledelayedexpansion

echo Starting build process for Timur Steam...

:: --- TOOLS CHECK ---

echo Checking for CMake...
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo CMake not found! Attempting to install via winget...
    winget install -e --id Kitware.CMake --source winget
    if !errorlevel! neq 0 (
        echo Failed to install CMake.
        pause
        exit /b 1
    )
    echo CMake installed. Please restart script.
    pause
    exit /b 0
)

echo Checking for Git...
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo Git not found! Attempting to install via winget...
    winget install -e --id Git.Git --source winget
    if !errorlevel! neq 0 (
        echo Failed to install Git.
        pause
        exit /b 1
    )
    echo Git installed.
)

:: --- COMPILER CHECK ---

echo Checking for C++ compiler...

:: 1. Check for valid environment (MSVC)
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo MSVC compiler found.
    goto :build_process
)

:: 2. Check for MinGW
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    echo MinGW compiler found.
    goto :build_process
)

:: 3. Try to setup Visual Studio Environment
echo Compiler not active. Attempting to auto-detect Visual Studio...

set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" (
    set "vswhere=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
)

if not exist "%vswhere%" (
    echo Could not find vswhere.exe.
    goto :no_compiler
)

:: Execute vswhere to find installation path
set "vs_path="
for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "vs_path=%%i"
)

if "!vs_path!"=="" (
    echo No suitable Visual Studio installation found.
    goto :no_compiler
)

echo Found Visual Studio at: !vs_path!
echo Setting up environment...

:: Call vcvars64.bat to setup environment variables
call "!vs_path!\VC\Auxiliary\Build\vcvars64.bat" >nul
if !errorlevel! neq 0 (
    echo Failed to initialize Visual Studio environment.
    goto :no_compiler
)

echo Environment initialized.
goto :build_process


:no_compiler
echo.
echo [ERROR] No C++ compiler found (cl.exe or g++).
echo.
echo Please install "Visual Studio Build Tools" with "Desktop development with C++".
echo Or run this script from a Developer Command Prompt.
echo.
pause
exit /b 1


:build_process
:: --- BUILD ---
echo.
echo Ready to build.

if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

mkdir build
cd build

echo Configuring CMake...
cmake ..
if !errorlevel! neq 0 (
    echo CMake configuration failed.
    echo Ensure Qt 6 is installed and CMAKE_PREFIX_PATH is set if needed.
    cd ..
    pause
    exit /b 1
)

echo Building project...
cmake --build . --config Release
if !errorlevel! neq 0 (
    echo Build failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo BUILD SUCCESSFUL
echo Executable: build\Release\Timur_steam.exe
echo ========================================
cd ..
pause
endlocal
