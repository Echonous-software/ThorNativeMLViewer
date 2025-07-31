#include <ui/UIManager.hpp>
#include <core/Error.hpp>
#include <data/DataManager.hpp>
#include <rendering/GLRenderer.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <algorithm>

namespace thor::ui {

UIManager::UIManager() 
    : mInitialized(false)
    , mUIState()
    , mDataManager(nullptr)
    , mGLRenderer(nullptr) {
}

UIManager::~UIManager() {
    shutdown();
}

bool UIManager::initialize(GLFWwindow* window) {
    if (mInitialized || !window) {
        return false;
    }
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 150";
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        throw thor::core::InitializationError("Failed to initialize ImGui GLFW backend");
    }
    
    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        ImGui_ImplGlfw_Shutdown();
        throw thor::core::InitializationError("Failed to initialize ImGui OpenGL3 backend");
    }
    
    mInitialized = true;
    return true;
}

void UIManager::shutdown() {
    if (mInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        mInitialized = false;
    }
}

void UIManager::newFrame() {
    if (!mInitialized) {
        return;
    }
    
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::render() {
    if (!mInitialized) {
        return;
    }
    
    // Render playback controls
    if (mUIState.showControls) {
        renderPlaybackControls();
    }
    
    // Render image display window
    if (mUIState.showImageWindow) {
        renderImageDisplay();
    }
    
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::renderPlaybackControls() {
    // Get the main viewport to position at bottom with full width
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    // Calculate window position and size
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    
    // Position at bottom of screen with full width
    ImVec2 window_pos = ImVec2(work_pos.x, work_pos.y + work_size.y);
    ImVec2 window_size = ImVec2(work_size.x, -1); // -1 for auto height
    
    // Set window position and size
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.0f, 1.0f)); // Anchor bottom-left
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
    
    // Window flags for bottom toolbar behavior
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | 
                                   ImGuiWindowFlags_NoMove | 
                                   ImGuiWindowFlags_NoCollapse |
                                   ImGuiWindowFlags_NoTitleBar;
    
    ImGui::Begin("##PlaybackControls", nullptr, window_flags);
    
    // Organize controls horizontally for better use of width
    
    // Left section: Play/pause and frame navigation
    renderPlayPauseButton();
    ImGui::SameLine();
    renderFrameNavigation();
    ImGui::SameLine();
    
    // Add some spacing
    ImGui::Text("  |  ");
    ImGui::SameLine();
    
    // Center section: Frame info
    renderFrameInfo();
    ImGui::SameLine();
    
    // Add some spacing
    ImGui::Text("  |  ");
    ImGui::SameLine();
    
    // Right section: FPS and window toggles
    renderFPSControl();
    ImGui::SameLine();
    
    ImGui::Text("  |  ");
    ImGui::SameLine();
    ImGui::Checkbox("Image Window", &mUIState.showImageWindow);
    
    ImGui::End();
}

void UIManager::renderImageDisplay() {
    ImGui::Begin("Image Display", &mUIState.showImageWindow, ImGuiWindowFlags_AlwaysAutoResize);
    
    if (mDataManager && mDataManager->hasSequence()) {
        auto imageView = mDataManager->getCurrentImageView();
        if (imageView.has_value()) {
            // Display image info
            ImGui::Text("Frame %u/%u", mUIState.currentFrame + 1, mUIState.totalFrames);
            ImGui::Text("Size: %ux%u, Channels: %u", 
                       imageView->getWidth(), 
                       imageView->getHeight(), 
                       imageView->getChannels());
            ImGui::Text("Type: %s", 
                       imageView->getPixelType() == thor::data::ImageDataType::UINT8 ? "UINT8" : "FLOAT32");
            
            // Display detected data range for float data
            if (imageView->getPixelType() == thor::data::ImageDataType::FLOAT32 && 
                mDataManager && mDataManager->hasDataRange()) {
                ImGui::Text("Data Range: %.3f to %.3f", 
                           mDataManager->getDataMinValue(), 
                           mDataManager->getDataMaxValue());
            }
            
            ImGui::Separator();
            
            // Min/Max Range Controls
            bool minChanged = false;
            bool maxChanged = false;
            
            minChanged = ImGui::InputFloat("Min Value", &mUIState.minValue, 0.1f, 1.0f, "%.1f");
            maxChanged = ImGui::InputFloat("Max Value", &mUIState.maxValue, 0.1f, 1.0f, "%.1f");
            
            if (ImGui::Button("Reset Range")) {
                mUIState.minValue = 0.0f;
                mUIState.maxValue = 1.0f;
                minChanged = true;
            }
            
            // Trigger callback if values changed
            if ((minChanged || maxChanged) && mMinMaxChangeCallback) {
                invokeCallbackSafely(mMinMaxChangeCallback, mUIState.minValue, mUIState.maxValue);
            }
        } else {
            ImGui::Text("No image data available");
        }
    } else {
        ImGui::Text("No image sequence loaded");
        ImGui::Text("Load an image sequence to see the display");
    }
    
    ImGui::End();
}



void UIManager::renderPlayPauseButton() {
    const char* buttonText = mUIState.isPlaying ? "Pause" : "Play";
    if (ImGui::Button(buttonText)) {
        invokeCallbackSafely(mPlayPauseCallback);
    }
}

void UIManager::renderFrameNavigation() {
    // Previous frame button
    bool canGoPrevious = mUIState.totalFrames > 0 && mUIState.currentFrame > 0;
    if (!canGoPrevious) ImGui::BeginDisabled();
    if (ImGui::Button("<<")) {
        invokeCallbackSafely(mPreviousFrameCallback);
    }
    if (!canGoPrevious) ImGui::EndDisabled();
    
    ImGui::SameLine();
    
    // Next frame button
    bool canGoNext = mUIState.totalFrames > 0 && mUIState.currentFrame < mUIState.totalFrames - 1;
    if (!canGoNext) ImGui::BeginDisabled();
    if (ImGui::Button(">>")) {
        invokeCallbackSafely(mNextFrameCallback);
    }
    if (!canGoNext) ImGui::EndDisabled();
}

void UIManager::renderFrameInfo() {
    // Compact frame counter display for bottom toolbar
    if (mUIState.totalFrames > 0) {
        ImGui::Text("Frame %u / %u", 
                   mUIState.currentFrame + 1, 
                   mUIState.totalFrames);
        
        // Add frame slider on same line with maximum available width
        if (mUIState.totalFrames > 1) {
            ImGui::SameLine();
            
            // Calculate available width for slider (window width minus used space)
            float availableWidth = ImGui::GetContentRegionAvail().x;
            // Reserve some space for the remaining UI elements (FPS control, separator, checkbox)
            float reservedWidth = 400.0f; // Approximate space needed for remaining elements
            float sliderWidth = std::max(150.0f, availableWidth - reservedWidth);
            
            ImGui::SetNextItemWidth(sliderWidth);
            int frameIndex = static_cast<int>(mUIState.currentFrame);
            int maxFrame = static_cast<int>(mUIState.totalFrames - 1);
            
            if (ImGui::SliderInt("##FrameSlider", &frameIndex, 0, maxFrame)) {
                uint32_t newFrame = static_cast<uint32_t>(frameIndex);
                if (newFrame != mUIState.currentFrame && mFrameSetCallback) {
                    invokeCallbackSafely(mFrameSetCallback, newFrame);
                }
            }
        }
    } else {
        ImGui::Text("No frames");
    }
}

void UIManager::renderFPSControl() {
    // Compact FPS control for bottom toolbar - sized for 4 digits
    float fps = mUIState.fps;
    ImGui::SetNextItemWidth(70.0f); // Width for 4 digits + decimal (e.g., "999.9")
    if (ImGui::InputFloat("FPS", &fps, 0.1f, 1.0f, "%.1f")) {
        mUIState.fps = fps;
        invokeCallbackSafely(mFPSChangeCallback, fps);
    }
}

void UIManager::setDataManager(thor::data::DataManager* dataManager) {
    mDataManager = dataManager;
}

void UIManager::setGLRenderer(thor::rendering::GLRenderer* glRenderer) {
    mGLRenderer = glRenderer;
}

void UIManager::updatePlaybackState(bool isPlaying, uint32_t currentFrame, uint32_t totalFrames) {
    mUIState.isPlaying = isPlaying;
    mUIState.currentFrame = currentFrame;
    mUIState.totalFrames = totalFrames;
}

void UIManager::updateRenderingParameters(const thor::rendering::RenderingParameters& params) {
    mUIState.minValue = params.minValue;
    mUIState.maxValue = params.maxValue;
}

void UIManager::showDemoWindow() {
    // Demo window functionality removed
}

void UIManager::invokeCallbackSafely(const std::function<void()>& callback) {
    if (callback) {
        try {
            callback();
        } catch (const std::exception& e) {
            // Log error but don't crash the UI
            // Could implement logging here
        }
    }
}

template<typename... Args>
void UIManager::invokeCallbackSafely(const std::function<void(Args...)>& callback, Args... args) {
    if (callback) {
        try {
            callback(args...);
        } catch (const std::exception& e) {
            // Log error but don't crash the UI
            // Could implement logging here
        }
    }
}

} // namespace thor::ui 