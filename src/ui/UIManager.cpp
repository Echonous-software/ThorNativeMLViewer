#include <ui/UIManager.hpp>
#include <core/Error.hpp>
#include <data/DataManager.hpp>
#include <rendering/GLRenderer.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

namespace thor::ui {

namespace {
    std::string formatMemorySize(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        }
        
        float size_kb = bytes / 1024.0f;
        if (size_kb < 1024.0f) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << size_kb << " KB";
            return ss.str();
        }
        
        float size_mb = size_kb / 1024.0f;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size_mb << " MB";
        return ss.str();
    }
}

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
    if (mUIState.showMetadataWindow) {
        renderMetadataDisplay();
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
    ImGui::Checkbox("Image Window", &mUIState.showMetadataWindow);
    
    ImGui::End();
}

void UIManager::renderMetadataDisplay() {
    ImGui::Begin("Metadata", &mUIState.showMetadataWindow, ImGuiWindowFlags_AlwaysAutoResize);
    
    if (mDataManager && mDataManager->hasSequence()) {
        auto imageView = mDataManager->getCurrentImageView();
        if (imageView.has_value()) {
            // Display image info
            ImGui::Text("Frame %u/%u", mUIState.currentFrame + 1, mUIState.totalFrames);
            ImGui::Text("Size: %ux%u, Channels: %u", 
                       imageView->getWidth(), 
                       imageView->getHeight(), 
                       imageView->getChannels());
            ImGui::Text("Memory: %s", formatMemorySize(imageView->getDataSizeBytes()).c_str());
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
            
            ImGui::Separator();
            
            // Zoom Controls
            renderZoomControls();
            
            ImGui::Separator();

            // Pixel Inspector
            renderPixelInspector();
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

void UIManager::renderZoomControls() {
    ImGui::Text("Zoom: %.1fx", mUIState.zoomFactor);
    
    if (ImGui::IsWindowHovered()) {
        if (ImGui::GetIO().MouseWheel > 0) zoomIn();
        if (ImGui::GetIO().MouseWheel < 0) zoomOut();

        if (mPixelInspectCallback) {
            invokeCallbackSafely(mPixelInspectCallback, ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Zoom In")) {
        zoomIn();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Zoom Out") || (ImGui::IsWindowHovered() && ImGui::GetIO().MouseWheel < 0)) {
        zoomOut();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Fit to Window")) {
        zoomToFit();
    }
    
    // Show zoom to fit status
    if (mUIState.isZoomToFit) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[Fit]");
    }
}

void UIManager::renderPixelInspector() {
    ImGui::Text("Pixel Inspector");
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mUIState.mousePosition.x, mUIState.mousePosition.y);

    if (mUIState.pixelValue.has_value()) {
        const auto& p_val = mUIState.pixelValue.value();
        if (p_val.size() == 1) {
            ImGui::Text("Pixel Value: %.3f", p_val[0]);
        } else if (p_val.size() == 3) {
            ImGui::Text("Pixel Value: (%.3f, %.3f, %.3f)", p_val[0], p_val[1], p_val[2]);
        } else if (p_val.size() == 4) {
            ImGui::Text("Pixel Value: (%.3f, %.3f, %.3f, %.3f)", p_val[0], p_val[1], p_val[2], p_val[3]);
        }
    } else {
        ImGui::Text("Pixel Value: N/A");
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

void UIManager::updatePixelInfo(const ImVec2& mousePosition, const std::optional<std::vector<float>>& pixelValue) {
    mUIState.mousePosition = mousePosition;
    mUIState.pixelValue = pixelValue;
}

void UIManager::showDemoWindow() {
    // Demo window functionality removed
}

void UIManager::zoomIn() {
    float newZoom = mUIState.zoomFactor * 1.25f;
    newZoom = std::min(newZoom, mUIState.zoomMax);
    if (newZoom != mUIState.zoomFactor) {
        mUIState.zoomFactor = newZoom;
        mUIState.isZoomToFit = false;
        invokeCallbackSafely(mZoomChangeCallback, mUIState.zoomFactor, mUIState.isZoomToFit);
    }
}

void UIManager::zoomOut() {
    float newZoom = mUIState.zoomFactor / 1.25f;
    newZoom = std::max(newZoom, mUIState.zoomMin);
    if (newZoom != mUIState.zoomFactor) {
        mUIState.zoomFactor = newZoom;
        mUIState.isZoomToFit = false;
        invokeCallbackSafely(mZoomChangeCallback, mUIState.zoomFactor, mUIState.isZoomToFit);
    }
}

void UIManager::zoomToFit() {
    mUIState.isZoomToFit = true;
    invokeCallbackSafely(mZoomChangeCallback, mUIState.zoomFactor, mUIState.isZoomToFit);
}

void UIManager::setZoom(float zoomFactor) {
    float newZoom = std::clamp(zoomFactor, mUIState.zoomMin, mUIState.zoomMax);
    bool zoomChanged = (newZoom != mUIState.zoomFactor);
    bool zoomToFitChanged = mUIState.isZoomToFit;
    
    mUIState.zoomFactor = newZoom;
    mUIState.isZoomToFit = false;
    
    // Trigger callback if either zoom factor or zoom-to-fit mode changed
    if (zoomChanged || zoomToFitChanged) {
        invokeCallbackSafely(mZoomChangeCallback, mUIState.zoomFactor, mUIState.isZoomToFit);
    }
}

void UIManager::handleMouseWheel(float yOffset) {
    if (yOffset > 0) {
        zoomIn();
    } else if (yOffset < 0) {
        zoomOut();
    }
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