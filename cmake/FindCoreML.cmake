# FindCoreML.cmake - Find CoreML framework on macOS

if(NOT APPLE)
    message(FATAL_ERROR "CoreML is only available on macOS")
endif()

set(CoreML_FRAMEWORK_PATH "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreML.framework")

find_library(CoreML_LIBRARIES
    NAMES CoreML
    PATHS ${CoreML_FRAMEWORK_PATH}
    NO_DEFAULT_PATH
)

find_path(CoreML_INCLUDE_DIRS
    NAMES CoreML.h
    PATHS ${CoreML_FRAMEWORK_PATH}/Headers
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CoreML
    REQUIRED_VARS CoreML_LIBRARIES CoreML_INCLUDE_DIRS
)

if(CoreML_FOUND)
    if(NOT TARGET CoreML::CoreML)
        add_library(CoreML::CoreML INTERFACE IMPORTED)
        set_target_properties(CoreML::CoreML PROPERTIES
            INTERFACE_LINK_LIBRARIES "${CoreML_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${CoreML_INCLUDE_DIRS}"
        )
    endif()
endif()

mark_as_advanced(CoreML_LIBRARIES CoreML_INCLUDE_DIRS) 