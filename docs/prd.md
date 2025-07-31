# ThorNativeMLViewer Product Requirements Document (PRD)

## Goals and Background Context

### Goals
- Enable rapid visual debugging of ML model conversions and post-processing pipelines
- Provide immediate visual feedback for model outputs (charts, bounding boxes, overlays) instead of numerical logs
- Catch common model conversion errors, particularly input normalization issues ([0-1] vs [0-255])
- Create a reusable tool that can scale from individual use to team-wide adoption
- Build MVP within one week timeframe with minimal but useful feature set

### Background Context

Machine learning developers currently lack effective tools for visually debugging model conversions and post-processing pipelines. When converting models between frameworks or debugging inference results, developers must parse through numerical logs and manually verify outputs, making it difficult to spot patterns or errors. This is particularly challenging when dealing with computer vision models where spatial relationships and probability distributions are better understood visually.

ThorNativeMLViewer addresses this gap by providing a native, high-performance visualization tool that enables real-time inspection of ML model outputs with immediate visual feedback. By leveraging C++ with OpenGL and ImGui, the tool provides the responsiveness needed for interactive debugging while maintaining the flexibility to support multiple ML backends beyond the initial CoreML implementation.

### Change Log

| Date | Version | Description | Author |
|------|---------|-------------|--------|
| [Today's Date] | 1.0 | Initial PRD Creation | [Your Name] |

## Requirements

### Functional

- FR1: Load and display images from flat uncompressed pixel arrays with support for common dimensions (128x128, 224x224, up to 1024x1024)
- FR2: Execute CoreML models on loaded images with configurable input normalization ([0-1] or [0-255] range)
- FR3: Display model inference results as confidence visualization for classification outputs (N values which sum to 1)
- FR4: Export processed data in NumPy format (.npy/.npz)
- FR5: Display performance metrics overlay showing FPS, inference time, and memory usage
- FR6: Support play, pause, and frame-by-frame navigation through sequential image data with configurable framerate (default 30 fps)

### Non Functional

- NFR1: Native application must run on macOS (MVP) with architecture supporting future Linux/Windows ports
- NFR2: UI must provide immediate visual feedback (<16ms) for parameter adjustments
- NFR3: Support pre-allocated fixed buffers to avoid memory allocation during processing
- NFR4: Architecture must support adding additional ML backends (ONNX, TensorRT) post-MVP
- NFR5: Application must handle images up to 1024x1024 without performance degradation
- NFR6: Provide extensible visualization system for adding new output types beyond MVP

## Technical Assumptions

**Repository Structure:** Monorepo

**Service Architecture:** Monolithic desktop application with modular internal architecture to support future ML backend additions

**Testing Requirements:** Unit testing using GoogleTest framework for critical components (model loading, data export, visualization logic)

**Additional Technical Assumptions and Requests:**

- **Build System:** Modern CMake (3.20+) with proper target-based dependencies
- **Package Management:** vcpkg for dependency management
- **Core Dependencies (MVP):**
  - ImGui for immediate mode UI
  - OpenGL 3.3 Core Profile for rendering
  - CoreML framework for model inference
  - fmt library for string formatting
- **Custom NPY/NPZ Writer:** Implement minimal subset needed for data export (avoid external dependencies)
- **Development Environment:** VS Code/Cursor with C++ extensions, CMake Tools, and LLDB debugger
- **Language Standard:** C++20 with modern features (smart pointers, RAII, std::filesystem, concepts where appropriate)
- **Platform Requirements:** macOS 11.0+ (for CoreML support)
- **Memory Management:** RAII with smart pointers (std::unique_ptr, std::shared_ptr), pre-allocated buffers for image processing
- **Error Handling:** C++ exceptions for error handling with clear error messages displayed in UI
- **Code Organization:** Header-only libraries where appropriate, clear separation between UI, ML backend, and visualization components

## Epic List

- Epic 1: Core ML Visualization Pipeline: Establish project foundation and implement complete image loading → model inference → visualization pipeline

## Epic 1: Core ML Visualization Pipeline

**Epic Goal:** Create a functional ML visualization tool that can load images, run CoreML classification models with configurable input normalization, visualize confidence scores, and export results. This MVP establishes the foundation for future enhancements while delivering immediate value for debugging ML model conversions.

### Story 1.1: Project Foundation and Build System

As a developer,
I want a properly configured C++ project with all dependencies,
so that I can build and run the application on macOS.

**Acceptance Criteria:**
1. CMake project structure created with Modern CMake best practices
2. vcpkg manifest file configured with imgui, opengl, and fmt dependencies
3. OpenGL 3.3 Core context initialization working with basic window
4. ImGui integrated and rendering demo window
5. GoogleTest framework integrated with sample test
6. Project builds and runs on macOS with single CMake command
7. Basic error handling structure with exceptions in place

### Story 1.2: Video Loading and Playback

As an ML developer,
I want to load and playback multi-frame image sequences,
so that I can analyze model behavior over time.

**Acceptance Criteria:**
1. Load sequence of images from float binary files of known width/height (Nx128x128 or Nx224x224)
2. Support uint8 and float32 input formats
3. Support scale and bias choice for float32 input format
4. Display current frame as texture in main window
5. Support frame-by-frame navigation (next/previous)
6. Implement play/pause functionality with looping
7. Display current frame number and total frames

### Story 1.3: Advanced Input Visualization

As an ML developer,
I want advanced visualization controls for input data,
so that I can inspect and understand my data in detail.

**Acceptance Criteria:**
1. Show a zoom control to zoom in and out on the main image
2. Display input metadata (dimensions, format, memory size)
3. Inspect a particular pixel value of input image by mouse, showing the original input value without scaling by min/max
4. Display performance metrics of fps and memory usage
5. Maintain original aspect ratio of image when scaling to fit display area
6. Always center the image within the display window
7. Provide "zoom to fit" functionality as the default zoom level

### Story 1.4: CoreML Model Integration

As an ML developer,
I want to load and run CoreML classification models,
so that I can test my model conversions.

**Acceptance Criteria:**
1. Load CoreML model (.mlpackage) from file system
2. Display model metadata (input shape, output shape, type)
3. Configure input normalization mode ([0-1] or [0-255])
4. Run inference on current frame
5. Extract classification outputs (N confidence values)
6. Display inference time in milliseconds
7. Handle model loading and inference errors gracefully

### Story 1.5: Confidence Visualization

As an ML developer,
I want to see model outputs as visual charts,
so that I can quickly understand classification results.

**Acceptance Criteria:**
1. Display confidence values as bar chart or histogram
2. Show class labels (if available) or indices
3. Highlight highest confidence class
4. Update visualization in real-time when running on new images
5. Ensure all confidence values sum to 1.0 (with tolerance)

### Story 1.6: Performance Metrics and Data Export

As an ML developer,
I want to monitor performance and export results,
so that I can optimize my models and share findings.

**Acceptance Criteria:**
1. Display FPS counter in UI overlay
2. Show per-frame inference time
3. Track and display memory usage
4. Export inference results to .npy format (current frame or all frames)
5. Success notification when export completes

## Checklist Results Report

### PM Checklist Validation

**Overall PRD Completeness: 92%**

**Category Status:**
- Problem Definition & Context: **PASS**
- MVP Scope Definition: **PASS**
- User Experience Requirements: **PARTIAL** (appropriate for technical tool)
- Functional Requirements: **PASS**
- Non-Functional Requirements: **PASS**
- Epic & Story Structure: **PASS**
- Technical Guidance: **PASS**
- Cross-Functional Requirements: **PASS**
- Clarity & Communication: **PASS**

**Critical Deficiencies:** None

**Minor Gaps (Acceptable for Internal Tool MVP):**
- Formal UI/UX specifications (ImGui handles this)
- Security requirements (internal tool)
- Deployment procedures (local development)

**Recommendations:**
1. None required - PRD is comprehensive and appropriate for a one-week MVP
2. The focused scope and clear technical direction support rapid development

### Final Decision

**READY FOR ARCHITECT**: The PRD is comprehensive, properly scoped for a one-week MVP, and ready for architectural design.

## Next Steps

### UX Expert Prompt

For this native C++ application using ImGui, traditional UI/UX specification is not required. The immediate mode GUI paradigm handles UI flow dynamically in code.

### Architect Prompt

Please create a technical architecture document for ThorNativeMLViewer using the architecture template. Focus on:
- Modular design supporting future ML backend additions (ONNX, TensorRT)
- Efficient memory management with pre-allocated buffers
- Clean separation between UI, ML inference, and visualization components
- OpenGL rendering pipeline integration with ImGui
- Exception-based error handling strategy
- Testing architecture with GoogleTest

The architecture should support the one-week MVP timeline while establishing patterns for future expansion.