#include <thor/app/Application.hpp>
#include <thor/rendering/GLContext.hpp>
#include <thor/ui/UIManager.hpp>
#include <thor/core/Error.hpp>
#include <iostream>

namespace thor::app {

Application::Application() 
    : mGLContext(std::make_unique<thor::rendering::GLContext>())
    , mUIManager(std::make_unique<thor::ui::UIManager>())
    , mInitialized(false) {
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    if (mInitialized) {
        return true;
    }
    
    try {
        // Initialize OpenGL context
        if (!mGLContext->initialize()) {
            throw thor::core::InitializationError("Failed to initialize OpenGL context");
        }
        
        // Initialize UI manager
        if (!mUIManager->initialize(mGLContext->getWindow())) {
            throw thor::core::InitializationError("Failed to initialize UI manager");
        }
        
        mInitialized = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Application initialization failed: " << e.what() << std::endl;
        shutdown();
        return false;
    }
}

int Application::run() {
    if (!mInitialized && !initialize()) {
        return -1;
    }
    
    try {
        // Main application loop
        while (!mGLContext->shouldClose()) {
            mGLContext->pollEvents();
            renderFrame();
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return -1;
    }
}

void Application::shutdown() {
    if (mUIManager) {
        mUIManager->shutdown();
    }
    
    if (mGLContext) {
        mGLContext->shutdown();
    }
    
    mInitialized = false;
}

void Application::renderFrame() {
    // Clear the screen
    mGLContext->clear();
    
    // Start ImGui frame
    mUIManager->newFrame();
    
    // Render ImGui
    mUIManager->render();
    
    // Swap buffers
    mGLContext->swapBuffers();
}

} // namespace thor::app 