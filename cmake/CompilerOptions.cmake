# CompilerOptions.cmake - Compiler-specific settings

# Enable all warnings and treat warnings as errors in debug builds
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Apple Clang specific optimizations
    if(APPLE)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
    endif()
endif()

# Position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Generate debug info for all build types
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Silence GL deprecation warnings
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
endif()