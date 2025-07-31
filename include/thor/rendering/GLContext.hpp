#pragma once

#include <memory>
#include <string>

struct GLFWwindow;

namespace thor::rendering {

class GLContext {
public:
    GLContext();
    ~GLContext();
    
    // Non-copyable, movable
    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;
    GLContext(GLContext&&) = default;
    GLContext& operator=(GLContext&&) = default;
    
    bool initialize(int width = 1024, int height = 768, const std::string& title = "ThorNativeMLViewer");
    void shutdown();
    
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void clear();
    GLFWwindow* getWindow() const { return mWindow; }
    bool isInitialized() const { return mInitialized; }
    
private:
    GLFWwindow* mWindow;
    bool mInitialized;
    
    static void errorCallback(int error, const char* description);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

} // namespace thor::rendering 