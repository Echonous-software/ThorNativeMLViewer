#pragma once

#include <memory>
#include <filesystem>

namespace thor::rendering {
    class GLContext;
    class GLRenderer;
}

namespace thor::ui {
    class UIManager;
}

namespace thor::data {
    class DataManager;
    enum class ImageDataType;
}

namespace thor::app {

class Application {
public:
    Application();
    ~Application();
    
    // Non-copyable, movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;
    
    bool initialize();
    int run();
    void shutdown();
    
    // Public interface for loading data
    bool loadImageSequence(const std::filesystem::path& filePath, 
                          uint32_t width, uint32_t height, 
                          thor::data::ImageDataType pixelType, 
                          uint32_t channels = 3);
    bool loadImageSequence128(const std::filesystem::path& filePath, 
                             thor::data::ImageDataType pixelType, 
                             uint32_t channels = 3);
    bool loadImageSequence224(const std::filesystem::path& filePath, 
                             thor::data::ImageDataType pixelType, 
                             uint32_t channels = 3);

private:
    // Core components
    std::unique_ptr<thor::rendering::GLContext> mGLContext;
    std::unique_ptr<thor::rendering::GLRenderer> mGLRenderer;
    std::unique_ptr<thor::ui::UIManager> mUIManager;
    std::unique_ptr<thor::data::DataManager> mDataManager;
    
    bool mInitialized;
    
    // Current texture for rendering
    uint32_t mCurrentTextureId;
    
    // Current rendering parameters
    float mRenderingMinValue;
    float mRenderingMaxValue;
    
    void renderFrame();
    void updateTexture();
    
    // UI callback handlers
    void onPlayPause();
    void onNextFrame();
    void onPreviousFrame();
    void onSetFrame(uint32_t frameIndex);
    void onFPSChange(float fps);
    void onMinMaxChange(float minValue, float maxValue);
    void onZoomChange(float zoomFactor, bool isZoomToFit);
    
    // Component initialization and wiring
    bool initializeComponents();
    void wireUICallbacks();
    void updateUIState();
};

} // namespace thor::app 