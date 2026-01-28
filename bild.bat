@echo off
setlocal enabledelayedexpansion

echo Starting build process for Timur Steam...

:: Check for CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo CMake not found! Attempting to install via winget...
    winget install -e --id Kitware.CMake --source winget
    if %errorlevel% neq 0 (
        echo Failed to install CMake. Please install it manually from https://cmake.org/download/
        pause
        exit /b 1
    )
    echo Please restart the script to refresh PATH or ensure CMake is available.
)

:: Check for Git
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo Git not found! Attempting to install via winget...
    winget install -e --id Git.Git --source winget
    if %errorlevel% neq 0 (
        echo Failed to install Git. Please install it manually.
        pause
        exit /b 1
    )
)

:: --- COMPILER CHECK ---
echo Checking for C++ compiler...

:: 1. Check if we are already in a VS Command Prompt (cl.exe exists)
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo MSVC compiler (cl.exe) found.
    goto :build_process
)

:: 2. Check for MinGW (g++)
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    echo MinGW compiler (g++) found.
    goto :build_process
)

:: 3. Try to find Visual Studio and set up environment
echo Compiler not active. Looking for Visual Studio...
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%vswhere%" (
    for /f "usebackq tokens=*" %%i in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "vs_path=%%i"
    )
    if defined vs_path (
        echo Found Visual Studio at "!vs_path!"
        echo Setting up build environment...
        call "!vs_path!\VC\Auxiliary\Build\vcvars64.bat" >nul
        if !errorlevel! equ 0 (
            echo Environment initialized successfully.
            goto :build_process
        )
    )
)

:: 4. If nothing works
echo.
echo [ERROR] No C++ compiler found!
echo CMake requires a compiler to build the project.
echo.
echo SOLUTIONS:
echo 1. Install "Visual Studio Build Tools" (C++ Desktop Development workload).
echo 2. OR Install MinGW (e.g. via Qt installer) and add it to PATH.
echo 3. OR Run this script from a "Developer Command Prompt for VS".
echo.
pause
exit /b 1

:build_process
echo.
echo NOTE: Ensure Qt 6 (WebEngine) is installed.
echo If CMake cannot find Qt, set CMAKE_PREFIX_PATH to your Qt lib folder.
echo.

:: Clean previous build
if exist build (
    echo Removing previous build directory...
    rmdir /s /q build
)

mkdir build
cd build

echo Configuring CMake...
cmake ..
if %errorlevel% neq 0 (
    echo CMake configuration failed. 
    echo Please check the error messages above.
    cd ..
    pause
    exit /b 1
)

echo Building...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed.
    cd ..
    pause
    exit /b 1
)

echo.
echo Build complete! Executable should be located in build\Release\Timur_steam.exe
cd ..
pause
endlocal
