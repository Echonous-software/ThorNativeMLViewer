# Source Tree

```plaintext
ThorNativeMLViewer/
├── CMakeLists.txt              # Root CMake configuration
├── vcpkg.json                  # vcpkg manifest
├── README.md                   # Project documentation
├── .gitignore
├── cmake/                      # CMake modules
│   └── CompilerOptions.cmake
├── src/                        # Implementation and header files
│   ├── main.cpp               # Application entry point
│   ├── app/                   # Application layer
│   │   ├── Application.cpp
│   │   └── Application.hpp
│   ├── ui/                    # UI components
│   │   ├── UIManager.cpp
│   │   ├── UIManager.hpp
│   │   ├── widgets/           # Custom ImGui widgets
│   │   │   ├── ChartWidget.cpp
│   │   │   └── ChartWidget.hpp
│   │   └── dialogs/           # Dialog implementations
│   ├── ml/                    # ML provider implementations
│   │   ├── MLProviderManager.cpp
│   │   ├── MLProviderManager.hpp
│   │   └── providers/
│   │       └── coreml/
│   │           ├── CoreMLProvider.mm
│   │           └── CoreMLProvider.hh
│   ├── data/                  # Data management
│   │   ├── DataManager.cpp
│   │   ├── DataManager.hpp
│   │   ├── ImageLoader.cpp
│   │   ├── ImageLoader.hpp
│   │   ├── ImageSequence.cpp
│   │   ├── ImageSequence.hpp
│   │   ├── DataExporter.cpp
│   │   └── DataExporter.hpp
│   ├── playback/              # Playback control
│   │   ├── PlaybackController.cpp
│   │   └── PlaybackController.hpp
│   ├── rendering/             # Rendering and graphics
│   │   ├── GLContext.cpp
│   │   ├── GLContext.hpp
│   │   ├── GLRenderer.cpp
│   │   ├── GLRenderer.hpp
│   │   ├── Texture.cpp
│   │   ├── Texture.hpp
│   │   └── shaders/           # GLSL shaders
│   │       ├── image.vert         # Vertex shader for image display
│   │       └── image.frag         # Fragment shader with scale/bias uniforms
│   └── core/                  # Core utilities
│       ├── Error.hpp              # Error handling and custom exceptions
│       ├── Math.hpp               # Custom math utilities
│       └── NPYWriter.cpp          # Custom NPY format writer
├── tests/                     # Unit tests
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── ml/
│   ├── data/
│   ├── playback/
│   └── core/
├── resources/                 # Application resources
│   ├── fonts/
│   └── icons/
├── data/                      # Binary data files and assets
│   ├── clips/                 # Image sequence binary files (*.bin)
│   ├── models/                # CoreML model files (*.mlpackage)
│   ├── samples/               # Sample/test data for development
│   └── exports/               # Runtime-generated export files (*.npy, *.npz)
└── docs/                      # Documentation
    ├── prd.md
    └── architecture.md
``` 