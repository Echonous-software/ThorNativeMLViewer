# Tech Stack

### Cloud Infrastructure

- **Provider:** N/A (Desktop Application)
- **Key Services:** Local filesystem only
- **Deployment Regions:** N/A

### Technology Stack Table

| Category | Technology | Version | Purpose | Rationale |
|----------|------------|---------|---------|-----------|
| **Language** | C++ | 20 | Primary development language | Modern features, performance, RAII |
| **Build System** | CMake | 3.20+ | Build configuration | Industry standard, cross-platform |
| **Package Manager** | vcpkg | latest | Dependency management | Microsoft-backed, CMake integration |
| **UI Framework** | Dear ImGui | 1.90+ | Immediate mode GUI | Responsive, ideal for debug tools |
| **Graphics API** | OpenGL | 3.3 Core | Rendering backend | Cross-platform, ImGui compatible |
| **ML Framework** | CoreML | 5.0+ | Initial ML backend | Native macOS performance |
| **String Formatting** | fmt | 10.0+ | Type-safe formatting | Better than printf, compile-time checks |
| **Testing** | GoogleTest | 1.14+ | Unit testing | Industry standard, good mocking |
| **Platform** | macOS | 11.0+ | Initial target platform | CoreML requirement |
| **Compiler** | Apple Clang | 14.0+ | C++ compiler | Best macOS support |
| **Debugger** | LLDB | bundled | Debugging | Native macOS debugger |
| **Documentation** | Doxygen | 1.14.0+ | In source documentation | Industry standard, widely compatible output |