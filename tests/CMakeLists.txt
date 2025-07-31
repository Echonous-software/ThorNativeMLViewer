# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Find GoogleTest
find_package(GTest CONFIG REQUIRED)

# Create test executable
add_executable(thor_tests)

# Set C++20 standard for tests
set_target_properties(thor_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(thor_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

# Link GoogleTest
target_link_libraries(thor_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        fmt::fmt
)

# Add test source files
target_sources(thor_tests
    PRIVATE
        test_main.cpp
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(thor_tests) 