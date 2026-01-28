#!/bin/bash

# Exit on error
set -e

echo "Starting build process for Timur Steam..."

# Install dependencies
# User is on Garuda (Arch-based), so we use pacman
if [ -f /etc/arch-release ]; then
    echo "Detected Arch Linux based system."
    echo "Installing dependencies..."
    sudo pacman -S --needed base-devel cmake git qt6-base qt6-webengine
elif command -v apt &> /dev/null; then
    # Fallback for Debian/Ubuntu users
    echo "Detected Debian/Ubuntu based system."
    echo "Installing dependencies..."
    sudo apt update
    sudo apt install -y build-essential cmake git qt6-base-dev qt6-webengine-dev libqt6webenginewidgets6-dev
elif command -v dnf &> /dev/null; then
    # Fallback for Fedora users
    echo "Detected Fedora based system."
    echo "Installing dependencies..."
    sudo dnf install -y cmake git qt6-qtbase-devel qt6-qtwebengine-devel gcc-c++
else
    echo "Could not detect package manager. Please ensure cmake, git, and qt6 (base + webengine) are installed."
fi

# Clean previous build
if [ -d "build" ]; then
    echo "Removing previous build directory..."
    rm -rf build
fi

# Create build directory
mkdir build
cd build

# Configure
echo "Configuring CMake..."
cmake ..

# Build
echo "Building project..."
cmake --build . -- -j$(nproc)

echo "Build complete! Run the following command to start:"
echo "./build/Timur_steam"
