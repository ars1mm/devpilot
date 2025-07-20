# DevPilot - Local AI Codebase Navigator

## 🧠 Overview

DevPilot is a local-first developer tool that helps engineers understand, navigate, and refactor large codebases using natural language, code graph analysis, and AI-powered summarization.

## 🚀 Quick Start (MVP)

### Build Requirements
- CMake 3.16+
- C++17 compiler (GCC/Clang)
- SQLite3 development libraries
- Git (for submodules)

### Build Instructions
```bash
# Clone with submodules
git clone --recursive https://github.com/yourusername/devpilot.git
cd devpilot

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Usage
```bash
# Index a C++ project
./devpilot index /path/to/cpp/project

# Search for symbols
./devpilot search "functionName"

# Find where a symbol is used
./devpilot usages "functionName"
```

## 📁 Project Structure

```
devpilot/
├── src/           # Core implementation (4 files only)
│   ├── parser.cpp # TreeSitter C++ parsing
│   ├── symbol.cpp # Symbol data structures
│   ├── storage.cpp# SQLite operations
│   └── main.cpp   # CLI interface
├── include/       # Header files
├── tests/         # Unit tests
├── external/      # Dependencies (TreeSitter, SQLite)
└── sample_projects/ # Test data
```

## 🎯 MVP Features

- **C++ Symbol Parsing**: Extract functions and classes from C++ files
- **Symbol Storage**: Store symbols in SQLite database
- **Symbol Search**: Find symbols by name
- **Usage Analysis**: Track function call relationships
- **CLI Interface**: Simple command-line tool

## 🔄 Development Philosophy

DevPilot follows the **single responsibility principle**:
- Each file serves exactly one purpose
- Clear separation of concerns
- Easy to test and maintain
- Scales from MVP to full system

## 📚 Documentation

See the `docs/` folder for comprehensive documentation:
- [System Architecture](docs/architecture/system-overview.md)
- [MVP Implementation](docs/mvp/README.md)
- [Coding Guidelines](docs/coding-guidelines.md)

## 🧪 Testing

```bash
# Build with tests
cmake -DBUILD_TESTS=ON ..
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

## 📄 License

MIT License - see LICENSE file for details.
