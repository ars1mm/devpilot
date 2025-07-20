cmake_minimum_required(VERSION 3.16)
project(devpilot VERSION 0.1.0 LANGUAGES CXX)

# Single responsibility: Only build the MVP executable
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)

# TreeSitter - check if available
find_path(TREE_SITTER_INCLUDE_DIR tree_sitter/api.h PATHS /usr/include /usr/local/include)
find_library(TREE_SITTER_LIBRARY tree-sitter PATHS /usr/lib /usr/local/lib)

if(TREE_SITTER_INCLUDE_DIR AND TREE_SITTER_LIBRARY)
    set(HAVE_TREE_SITTER TRUE)
    message(STATUS "TreeSitter found: ${TREE_SITTER_LIBRARY}")
else()
    set(HAVE_TREE_SITTER FALSE)
    message(WARNING "TreeSitter not found. Building with stub parser (limited functionality)")
endif()

# DevPilot executable
add_executable(devpilot
    src/main.cpp
    src/parser.cpp
    src/symbol.cpp
    src/storage.cpp
)

# Include directories
target_include_directories(devpilot PRIVATE include)

# Link libraries
target_link_libraries(devpilot SQLite::SQLite3)

# If TreeSitter is available, link it and add compile definition
if(HAVE_TREE_SITTER)
    target_include_directories(devpilot PRIVATE ${TREE_SITTER_INCLUDE_DIR})
    target_link_libraries(devpilot ${TREE_SITTER_LIBRARY})
    target_compile_definitions(devpilot PRIVATE HAVE_TREE_SITTER)
    message(STATUS "Building with TreeSitter support")
else()
    target_compile_definitions(devpilot PRIVATE NO_TREE_SITTER)
    message(STATUS "Building with stub parser (no TreeSitter)")
endif()

# Enable filesystem support (needed for C++17 std::filesystem)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(devpilot stdc++fs)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(devpilot PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests (optional)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS devpilot RUNTIME DESTINATION bin)

# Print build information
message(STATUS "DevPilot MVP build configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  SQLite3: ${SQLite3_FOUND}")
message(STATUS "  TreeSitter: ${HAVE_TREE_SITTER}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
