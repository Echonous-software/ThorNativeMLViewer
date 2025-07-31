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
        ${CMAKE_SOURCE_DIR}/src/third_party/glad/include
)

# Link GoogleTest and required libraries
target_link_libraries(thor_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        fmt::fmt
        glfw
        ${OPENGL_LIBRARIES}
)

# Platform-specific libraries for tests
if(APPLE)
    target_link_libraries(thor_tests
        PRIVATE
            "-framework CoreML"
    )
endif()

# Add test source files
target_sources(thor_tests
    PRIVATE
        test_main.cpp
        test_error.cpp
        data/test_image_sequence.cpp
        data/test_image_loader.cpp
        data/test_data_manager.cpp
        playback/test_playback_controller.cpp
        rendering/test_gl_renderer.cpp
        # Source files needed for testing
        ${CMAKE_SOURCE_DIR}/src/data/ImageSequence.cpp
        ${CMAKE_SOURCE_DIR}/src/data/ImageLoader.cpp
        ${CMAKE_SOURCE_DIR}/src/data/DataManager.cpp
        ${CMAKE_SOURCE_DIR}/src/playback/PlaybackController.cpp
        ${CMAKE_SOURCE_DIR}/src/rendering/GLContext.cpp
        ${CMAKE_SOURCE_DIR}/src/rendering/GLRenderer.cpp
        ${CMAKE_SOURCE_DIR}/src/third_party/glad/src/glad.c
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(thor_tests) 