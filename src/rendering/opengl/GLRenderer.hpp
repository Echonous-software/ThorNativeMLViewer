#pragma once

#include <rendering/Renderer.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace echonous {

class GLRenderer : public Renderer {

    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> mWindow;

public:
    GLRenderer(std::string_view name, IVec2 windowSize);
    ~GLRenderer();

    std::string_view name() const override;
    void beginFrame() override;
    void render(const RenderState& state) override;
    void endFrame() override;
    bool shouldClose() const override;
    void close() override;


private:
    void initializeGLFW();
    void initializeImGuiBackend();
    void openWindow(std::string_view title, IVec2 size);
    void createContext();

    void sizeChanged(IVec2 size);

    static void errorCallback(int error, const char* description);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

};

} // namespace echonous