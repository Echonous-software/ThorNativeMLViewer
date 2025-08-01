#pragma once

#include <functional>
#include <optional>
#include <vector>

#include <imgui.h>

// Forward declarations
struct GLFWwindow;

namespace thor::data {
    class DataManager;
}

namespace thor::rendering {
    class GLRenderer;
    struct RenderingParameters;
}

namespace thor::ui {

// UI state for playback controls
struct PlaybackUIState {
    bool isPlaying = false;
    uint32_t currentFrame = 0;
    uint32_t totalFrames = 0;
    float fps = 30.0f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    bool showControls = true;
    bool showMetadataWindow = true;
    
    // Zoom controls
    float zoomFactor = 1.0f;
    bool isZoomToFit = true;
    float zoomMin = 0.1f;
    float zoomMax = 10.0f;

    // Pixel inspector
    ImVec2 mousePosition;
    std::optional<std::vector<float>> pixelValue;
};

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    // Non-copyable, movable
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager(UIManager&&) = default;
    UIManager& operator=(UIManager&&) = default;
    
    bool initialize(GLFWwindow* window);
    void shutdown();
    
    void newFrame();
    void render();
    
    // Playback UI controls
    void renderPlaybackControls();
    void renderMetadataDisplay();
    
    // Integration with data and rendering components
    void setDataManager(thor::data::DataManager* dataManager);
    void setGLRenderer(thor::rendering::GLRenderer* glRenderer);
    
    // UI state access
    const PlaybackUIState& getUIState() const { return mUIState; }
    PlaybackUIState& getUIState() { return mUIState; }
    
    // Callbacks for UI events
    void setPlayPauseCallback(std::function<void()> callback) { mPlayPauseCallback = callback; }
    void setNextFrameCallback(std::function<void()> callback) { mNextFrameCallback = callback; }
    void setPreviousFrameCallback(std::function<void()> callback) { mPreviousFrameCallback = callback; }
    void setFrameSetCallback(std::function<void(uint32_t)> callback) { mFrameSetCallback = callback; }
    void setFPSChangeCallback(std::function<void(float)> callback) { mFPSChangeCallback = callback; }
    void setMinMaxChangeCallback(std::function<void(float, float)> callback) { mMinMaxChangeCallback = callback; }
    void setZoomChangeCallback(std::function<void(float, bool)> callback) { mZoomChangeCallback = callback; }
    void setPixelInspectCallback(std::function<void(float, float)> callback) { mPixelInspectCallback = callback; }
    
    // Update UI state from external sources
    void updatePlaybackState(bool isPlaying, uint32_t currentFrame, uint32_t totalFrames);
    void updateRenderingParameters(const thor::rendering::RenderingParameters& params);
    void updatePixelInfo(const ImVec2& mousePosition, const std::optional<std::vector<float>>& pixelValue);
    
    // Zoom control methods
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void setZoom(float zoomFactor);
    void handleMouseWheel(float yOffset);
    
    void showDemoWindow();

private:
    bool mInitialized;
    
    // UI state
    PlaybackUIState mUIState;
    
    // Component references
    thor::data::DataManager* mDataManager;
    thor::rendering::GLRenderer* mGLRenderer;
    
    // Callbacks
    std::function<void()> mPlayPauseCallback;
    std::function<void()> mNextFrameCallback;
    std::function<void()> mPreviousFrameCallback;
    std::function<void(uint32_t)> mFrameSetCallback;
    std::function<void(float)> mFPSChangeCallback;
    std::function<void(float, float)> mMinMaxChangeCallback;
    std::function<void(float, bool)> mZoomChangeCallback;
    std::function<void(float, float)> mPixelInspectCallback;
    
    // Helper methods
    void renderPlayPauseButton();
    void renderFrameNavigation();
    void renderFrameInfo();
    void renderFPSControl();
    void renderZoomControls();
    void renderPixelInspector();
    void invokeCallbackSafely(const std::function<void()>& callback);
    template<typename... Args>
    void invokeCallbackSafely(const std::function<void(Args...)>& callback, Args... args);
};

} // namespace thor::ui 