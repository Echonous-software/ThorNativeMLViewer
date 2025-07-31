#pragma once

#include <memory>

namespace thor::rendering {
    class GLContext;
}

namespace thor::ui {
    class UIManager;
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
    
private:
    std::unique_ptr<thor::rendering::GLContext> mGLContext;
    std::unique_ptr<thor::ui::UIManager> mUIManager;
    bool mInitialized;
    
    void renderFrame();
};

} // namespace thor::app 