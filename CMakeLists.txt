cmake_minimum_required(VERSION 3.20)

project(ThorNativeMLViewer 
    VERSION 1.0.0
    DESCRIPTION "Native ML model visualization and debugging tool"
    LANGUAGES C CXX)

# vcpkg integration
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
# Ignore GL deprecation warnings on macOS
if(APPLE)
    get_target_property(THOR_COMPILE_DEFINITIONS ${PROJECT_NAME} COMPILE_DEFINITIONS)
    if (THOR_COMPILE_DEFINITIONS)
        list(APPEND THOR_COMPILE_DEFINITIONS "GL_SILENCE_DEPRECATION")
    else()
        set(THOR_COMPILE_DEFINITIONS "GL_SILENCE_DEPRECATION")
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_DEFINITIONS "${THOR_COMPILE_DEFINITIONS}"
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/glad/include
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
        src/app/Application.cpp
        src/app/AppState.cpp
        src/app/PlaybackController.cpp
        src/core/Math.cpp
        src/data/ByteOrder.cpp
        src/data/DataError.cpp
        src/data/DataManager.cpp
        src/data/DataType.cpp
        src/data/ImageSequence.cpp
        src/data/ImageView.cpp
        src/data/ImageSize.cpp
        src/rendering/opengl/GLRenderer.cpp
        src/rendering/Renderer.cpp
        src/ui/UserInterface.cpp
        src/third_party/glad/src/glad.c
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