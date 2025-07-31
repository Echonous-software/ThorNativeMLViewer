# Source Tree

```plaintext
ThorNativeMLViewer/
├── CMakeLists.txt              # Root CMake configuration
├── vcpkg.json                  # vcpkg manifest
├── README.md                   # Project documentation
├── .gitignore
├── cmake/                      # CMake modules
│   ├── FindCoreML.cmake
│   └── CompilerOptions.cmake
├── include/                    # Public headers
│   └── thor/
│       ├── core/              # Core interfaces
│       │   ├── Types.hpp
│       │   ├── Tensor.hpp
│       │   └── Error.hpp
│       ├── ml/                # ML provider interfaces
│       │   ├── MLProvider.hpp
│       │   ├── Model.hpp
│       │   └── InferenceResult.hpp
│       └── rendering/         # Rendering interfaces
│           ├── Renderer.hpp
│           └── Texture.hpp
├── src/                       # Implementation files
│   ├── main.cpp              # Application entry point
│   ├── app/                  # Application layer
│   │   ├── Application.cpp
│   │   └── Application.hpp
│   ├── ui/                   # UI components
│   │   ├── UIManager.cpp
│   │   ├── UIManager.hpp
│   │   ├── widgets/          # Custom ImGui widgets
│   │   │   ├── ChartWidget.cpp
│   │   │   └── ChartWidget.hpp
│   │   └── dialogs/          # Dialog implementations
│   ├── ml/                   # ML provider implementations
│   │   ├── MLProviderManager.cpp
│   │   ├── MLProviderManager.hpp
│   │   └── providers/
│   │       └── coreml/
│   │           ├── CoreMLProvider.mm
│   │           └── CoreMLProvider.hh
│   ├── data/                 # Data management
│   │   ├── DataManager.cpp
│   │   ├── DataManager.hpp
│   │   ├── ImageLoader.cpp
│   │   ├── ImageLoader.hpp
│   │   ├── ImageSequence.cpp
│   │   ├── ImageSequence.hpp
│   │   ├── DataExporter.cpp
│   │   └── DataExporter.hpp
│   ├── playback/             # Playback control
│   │   ├── PlaybackController.cpp
│   │   └── PlaybackController.hpp
│   ├── rendering/            # Rendering and graphics
│   │   ├── GLContext.cpp
│   │   ├── GLContext.hpp
│   │   ├── Renderer.cpp
│   │   ├── Renderer.hpp
│   │   ├── Texture.cpp
│   │   ├── Texture.hpp
│   │   └── shaders/          # GLSL shaders
│   │       ├── basic.vert
│   │       └── basic.frag
│   └── core/                 # Core utilities
│       ├── Math.hpp            # Custom math utilities
│       └── NPYWriter.cpp     # Custom NPY format writer
├── tests/                    # Unit tests
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── ml/
│   ├── data/
│   ├── playback/
│   └── core/
├── resources/                # Application resources
│   ├── fonts/
│   └── icons/
└── docs/                     # Documentation
    ├── prd.md
    └── architecture.md
``` 