@echo off
setlocal

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

:: Note about Qt
echo.
echo NOTE: This script assumes Qt 6 is installed and configured in your environment.
echo If the build fails, verify that you have Qt 6 installed (including WebEngine) and CMAKE_PREFIX_PATH is set correctly.
echo.

:: Clean previous build
if exist build (
    echo Removing previous build directory...
    rmdir /s /q build
)

mkdir build
cd build

echo Configuring CMake...
:: Attempt basic configuration. 
:: If Qt is not in PATH, this might fail unless CMAKE_PREFIX_PATH is set.
cmake ..
if %errorlevel% neq 0 (
    echo CMake configuration failed. 
    echo Please make sure you are running this from a suitable environment (e.g., Qt environment shell) 
    echo or have CMAKE_PREFIX_PATH pointing to your Qt lib directory.
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
