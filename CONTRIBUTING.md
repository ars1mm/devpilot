# Contributing to DevPilot

Thank you for your interest in contributing to DevPilot! This document provides guidelines for contributing to the project.

## Development Setup

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler (GCC 8+, Clang 10+, MSVC 2019+)
- SQLite3 development libraries
- Tree-sitter library (optional, for enhanced parsing)
- pkg-config

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/ars1mm/devpilot.git
   cd devpilot/devpilot
   ```

2. Install dependencies:
   
   **Ubuntu/Debian:**
   ```bash
   sudo apt-get install build-essential cmake libsqlite3-dev libtree-sitter-dev pkg-config
   ```
   
   **macOS:**
   ```bash
   brew install cmake sqlite tree-sitter pkg-config
   ```
   
   **Windows:**
   ```bash
   # Using vcpkg
   vcpkg install sqlite3 tree-sitter
   ```

3. Build the project:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
   cmake --build build
   ```

4. Run tests:
   ```bash
   cd build && ctest --output-on-failure
   ```

## Code Style

- Follow the existing code style in the project
- Use the provided `.clang-format` configuration
- Run `clang-format` before committing:
  ```bash
  find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
  ```

## Testing

- Write tests for new functionality
- Ensure all existing tests pass before submitting a PR
- Add integration tests for major features
- Use descriptive test names and assertions

## Pull Request Process

1. Fork the repository
2. Create a feature branch from `main`:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. Make your changes following the coding guidelines
4. Add or update tests as necessary
5. Ensure all tests pass and code is properly formatted
6. Update documentation if needed
7. Submit a pull request with:
   - Clear description of the changes
   - Reference to any related issues
   - Screenshots/examples if applicable

## Commit Messages

Use conventional commit format:
- `feat: add new feature`
- `fix: resolve bug in parser`
- `docs: update README`
- `test: add unit tests for storage`
- `refactor: improve symbol indexing`
- `ci: update GitHub Actions workflow`

## Code Review Guidelines

- Be respectful and constructive
- Focus on the code, not the person
- Explain your suggestions clearly
- Test the changes locally when possible

## Reporting Issues

Please use the issue templates provided:
- Bug reports: Include steps to reproduce, expected vs actual behavior
- Feature requests: Describe the use case and proposed solution

## Project Structure

```
devpilot/
├── src/           # Core C++ implementation
├── include/       # Header files
├── tests/         # Unit and integration tests
├── external/      # Third-party dependencies
└── sample_projects/ # Example projects for testing
```

## Areas for Contribution

We welcome contributions in these areas:
- Core parsing engine improvements
- Additional language support
- Performance optimizations
- Documentation and examples
- Testing and CI/CD improvements
- Bug fixes and stability improvements

## Getting Help

- Check existing issues and documentation
- Join discussions in GitHub issues
- Ask questions in pull requests

Thank you for contributing to DevPilot!
