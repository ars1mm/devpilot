# DevPilot External Dependencies

## Required Dependencies

DevPilot MVP requires the following external libraries:

### 1. SQLite3
**Purpose**: Database storage for symbols and call relationships
**Installation**:
```bash
# Ubuntu/Debian
sudo apt-get install libsqlite3-dev

# macOS (Homebrew)
brew install sqlite3

# Windows (vcpkg)
vcpkg install sqlite3
```

### 2. TreeSitter
**Purpose**: Source code parsing
**Installation**:

#### Option A: System Package (Recommended for testing)
```bash
# Ubuntu/Debian
sudo apt-get install libtree-sitter-dev

# macOS (Homebrew)
brew install tree-sitter
```

#### Option B: As Git Submodule (Recommended for distribution)
```bash
# From devpilot project root
git submodule add https://github.com/tree-sitter/tree-sitter.git external/tree-sitter
git submodule add https://github.com/tree-sitter/tree-sitter-cpp.git external/tree-sitter-cpp
git submodule update --init --recursive
```

#### Option C: Manual Build
```bash
# Clone and build TreeSitter
git clone https://github.com/tree-sitter/tree-sitter.git external/tree-sitter
cd external/tree-sitter
make
sudo make install

# Clone and build TreeSitter C++ grammar
git clone https://github.com/tree-sitter/tree-sitter-cpp.git external/tree-sitter-cpp
cd external/tree-sitter-cpp
npm install
npm run build
```

## Quick Setup for Development

For the fastest setup to start developing:

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y cmake g++ libsqlite3-dev libtree-sitter-dev pkg-config

# Clone the project
cd /home/arsim/Desktop/github/devpilot
cd devpilot

# Build
./build.sh

# Test with sample project
./build/devpilot index sample_projects/cpp_example
./build/devpilot search "Calculator"
```

## Dependency Management Strategy

### MVP Phase
- Use system packages for quick development
- Minimal external dependencies
- Focus on core functionality

### Production Phase
- Add dependencies as git submodules
- Static linking for distribution
- Cross-platform build support

## Notes

- TreeSitter is optional for initial testing - the project will build without it but parsing won't work
- SQLite3 is required - the project won't build without it
- All dependencies follow the single responsibility principle in our build system
