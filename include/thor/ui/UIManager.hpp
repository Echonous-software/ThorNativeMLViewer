#pragma once

#include <memory>

struct GLFWwindow;

namespace thor::ui {

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
    
    void showDemoWindow();
    
private:
    bool mInitialized;
    bool mShowDemo;
};

} // namespace thor::ui 