#!/bin/bash
# Single responsibility: Only build the DevPilot MVP

set -e

echo "Building DevPilot MVP..."

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Run this script from the devpilot project root."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building..."
make -j$(nproc)

# Check if build was successful
if [ -f "devpilot" ]; then
    echo "Build successful! DevPilot executable created."
    echo "Run './build/devpilot help' to see usage information."
else
    echo "Build failed - executable not found."
    exit 1
fi

echo "DevPilot MVP build complete!"
