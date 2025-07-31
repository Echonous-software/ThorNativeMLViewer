#include <thor/ui/UIManager.hpp>
#include <thor/core/Error.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace thor::ui {

UIManager::UIManager() : mInitialized(false), mShowDemo(true) {
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
    
    // Show demo window if enabled
    if (mShowDemo) {
        showDemoWindow();
    }
    
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::showDemoWindow() {
    ImGui::ShowDemoWindow(&mShowDemo);
}

} // namespace thor::ui 