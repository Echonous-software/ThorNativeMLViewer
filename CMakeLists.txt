cmake_minimum_required(VERSION 3.20)

project(ThorNativeMLViewer 
    VERSION 1.0.0
    DESCRIPTION "Native ML model visualization and debugging tool"
    LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include compiler options
include(CompilerOptions)

# vcpkg integration
find_package(PkgConfig REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Create main executable target
add_executable(${PROJECT_NAME})

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        fmt::fmt
        imgui::imgui
        glfw
        ${OPENGL_LIBRARIES}
)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            "-framework CoreML"
    )
endif()

# Add source files (will be added as we create them)
target_sources(${PROJECT_NAME}
    PRIVATE
        src/main.cpp
        src/rendering/GLContext.cpp
        src/ui/UIManager.cpp
        src/app/Application.cpp
)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
endif() 