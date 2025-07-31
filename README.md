# ThorNativeMLViewer

Native ML model visualization and debugging tool built with C++20.

## Features

- OpenGL 3.3 Core Profile rendering
- Dear ImGui immediate mode GUI
- CoreML model support (macOS)
- Real-time ML model visualization
- Cross-platform architecture (starting with macOS)

## Requirements

- macOS 11.0+ 
- Apple Clang 14.0+
- CMake 3.20+
- vcpkg (for dependency management)

## Dependencies

- Dear ImGui 1.90+
- OpenGL 3.3 Core
- GLFW 3.3+
- fmt 10.0+
- GoogleTest 1.14+
- CoreML 5.0+

## Building

1. Install vcpkg and set up the toolchain
2. Build with CMake:

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

## Running

```bash
./ThorNativeMLViewer
```

## Testing

```bash
cd build
ctest
```

## License

TBD 