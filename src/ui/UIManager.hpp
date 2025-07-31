#pragma once

#include <functional>
#include <optional>

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
    bool showImageWindow = true;
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
    void renderImageDisplay();
    
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
    
    // Update UI state from external sources
    void updatePlaybackState(bool isPlaying, uint32_t currentFrame, uint32_t totalFrames);
    void updateRenderingParameters(const thor::rendering::RenderingParameters& params);
    
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
    
    // Helper methods
    void renderPlayPauseButton();
    void renderFrameNavigation();
    void renderFrameInfo();
    void renderFPSControl();
    void invokeCallbackSafely(const std::function<void()>& callback);
    template<typename... Args>
    void invokeCallbackSafely(const std::function<void(Args...)>& callback, Args... args);
};

} // namespace thor::ui 