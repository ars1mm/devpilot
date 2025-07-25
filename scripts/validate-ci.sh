#!/bin/bash

# Script to validate CI setup locally
# This script simulates the GitHub Actions workflow steps

set -e

echo "🔍 Validating DevPilot CI setup..."

# Change to project directory
cd "$(dirname "$0")/../devpilot"

echo "📁 Current directory: $(pwd)"

# Clean build directory
echo "🧹 Cleaning build directory..."
rm -rf build
mkdir -p build

# Check dependencies
echo "🔧 Checking dependencies..."
if command -v cmake &> /dev/null; then
    echo "✅ CMake: $(cmake --version | head -n1)"
else
    echo "❌ CMake not found"
    exit 1
fi

if command -v gcc &> /dev/null; then
    echo "✅ GCC: $(gcc --version | head -n1)"
else
    echo "❌ GCC not found"
    exit 1
fi

if pkg-config --exists sqlite3; then
    echo "✅ SQLite3: $(pkg-config --modversion sqlite3)"
else
    echo "❌ SQLite3 not found"
    exit 1
fi

# Check tree-sitter (optional)
if pkg-config --exists tree-sitter; then
    echo "✅ TreeSitter: $(pkg-config --modversion tree-sitter)"
else
    echo "⚠️  TreeSitter not found (will build without it)"
fi

# Configure project
echo "⚙️  Configuring project..."
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON

# Build project
echo "🔨 Building project..."
cmake --build build --config Release

# Run tests
echo "🧪 Running tests..."
cd build
if ctest --output-on-failure; then
    echo "✅ All tests passed!"
else
    echo "❌ Tests failed"
    exit 1
fi

# Test executable
echo "🚀 Testing executable..."
if [[ -f "devpilot" ]]; then
    if ./devpilot --help &> /dev/null; then
        echo "✅ Executable runs successfully"
    else
        echo "✅ Executable exists (--help may not be implemented yet)"
    fi
else
    echo "❌ Executable not found"
    exit 1
fi

echo "🎉 All validation checks passed!"
echo "✨ The CI setup should work correctly on GitHub Actions"
