cmake_minimum_required(VERSION 3.16)

# Simple test framework - can be replaced with Google Test later
add_executable(test_basic
    test_basic.cpp
)

target_include_directories(test_basic PRIVATE ../include)
target_link_libraries(test_basic SQLite::SQLite3)

# If TreeSitter is available, link it
if(HAVE_TREE_SITTER)
    target_include_directories(test_basic PRIVATE ${TREE_SITTER_INCLUDE_DIR})
    target_link_libraries(test_basic ${TREE_SITTER_LIBRARY})
    target_compile_definitions(test_basic PRIVATE HAVE_TREE_SITTER)
else()
    target_compile_definitions(test_basic PRIVATE NO_TREE_SITTER)
endif()

# Add filesystem support for older GCC
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
    target_link_libraries(test_basic stdc++fs)
endif()

# Register test with CTest
add_test(NAME BasicTests COMMAND test_basic)

# Set test properties
set_tests_properties(BasicTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
