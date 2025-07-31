# Coding Standards

### Core Standards

- **Languages & Runtimes:** C++20 (Apple Clang 14.0+)
- **Style & Linting:** .clang-format with Google style base
- **Test Organization:** Mirror source structure in tests/

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes | PascalCase | `BufferManager` |
| Functions | camelCase | `loadModel()` |
| Member Variables | mCamelCase | `mBufferSize` |
| Constants | UPPER_SNAKE | `MAX_TEXTURE_SIZE` |
| Namespaces | lowercase | `thor::ml` |

### Critical Rules

- **Memory Management:** Always use smart pointers, never raw new/delete
- **OpenGL Resources:** Wrap in RAII classes with proper cleanup
- **OpenGL Headers:** Never include OpenGL headers directly (GL/gl.h, OpenGL/gl.h), always use GLFW headers (GLFW/glfw3.h)
- **Rendering Separation:** OpenGL calls must only occur in rendering-specific files (src/rendering/), never in Application or other non-rendering components
- **Data Integrity:** Store raw pixel data unchanged in memory. Apply scale/bias transformations via GLSL shaders during rendering only
- **Buffer Access:** Always bounds-check, use span where possible
- **Error Handling:** Throw exceptions for runtime errors, never return error codes. Use asserts for logic errors
- **CoreML Objects:** Wrap in C++ MLModel class, do not use Objective-C anywhere other than in CoreML backend adapter

### Language-Specific Guidelines

#### C++ Specifics

- **File naming conventions:** Use `.hpp` and `.cpp` for C++. Use `.hh` and `.mm` for Objective-C++
- **Include guard:** Use `#pragma once` as an include guard in all header files
- **Modern Features:** Use span and string_view
- **Const Correctness:** Mark everything const that can be
- **Rule of Zero:** Prefer using default copy/move constructors and assignment operators where possible
