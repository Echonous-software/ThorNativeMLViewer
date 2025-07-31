#include <rendering/GLContext.hpp>
#include <core/Error.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fmt/format.h>

namespace thor::rendering {

GLContext::GLContext() : mWindow(nullptr), mInitialized(false) {
}

GLContext::~GLContext() {
    shutdown();
}

bool GLContext::initialize(int width, int height, const std::string& title) {
    if (mInitialized) {
        return true;
    }
    
    // Set error callback
    glfwSetErrorCallback(errorCallback);
    
    // Initialize GLFW
    if (!glfwInit()) {
        throw thor::core::InitializationError("Failed to initialize GLFW");
    }
    
    // Configure GLFW for OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        throw thor::core::OpenGLError("Failed to create GLFW window");
    }
    
    // Make context current
    glfwMakeContextCurrent(mWindow);
    
    // Set callbacks
    glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
    
    // Enable v-sync
    glfwSwapInterval(1);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        shutdown();
        throw thor::core::OpenGLError("Failed to initialize GLAD");
    }
    
    // Verify OpenGL version
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (!version) {
        shutdown();
        throw thor::core::OpenGLError("Failed to get OpenGL version");
    }
    
    mInitialized = true;
    return true;
}

void GLContext::shutdown() {
    if (mWindow) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    if (mInitialized) {
        glfwTerminate();
        mInitialized = false;
    }
}

bool GLContext::shouldClose() const {
    return mWindow ? glfwWindowShouldClose(mWindow) : true;
}

void GLContext::swapBuffers() {
    if (mWindow) {
        glfwSwapBuffers(mWindow);
    }
}

void GLContext::pollEvents() {
    glfwPollEvents();
}

void GLContext::errorCallback(int error, const char* description) {
    throw thor::core::OpenGLError(fmt::format("GLFW Error {}: {}", error, description));
}

void GLContext::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void GLContext::clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace thor::rendering 