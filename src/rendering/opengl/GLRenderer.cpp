#include <rendering/opengl/GLRenderer.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace echonous {

GLRenderer::GLRenderer(std::string_view name, IVec2 windowSize) :
    mWindow(nullptr, glfwDestroyWindow)
{
    initializeGLFW();
    openWindow(name, windowSize);
    createContext();
    initializeImGuiBackend();
}

GLRenderer::~GLRenderer() {
    close();
}

std::string_view GLRenderer::name() const {
    return "OpenGL";
}

void GLRenderer::beginFrame() {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::render(const RenderState& state) {

    bool show_demo_window = true;
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GLRenderer::endFrame() {
    glfwSwapBuffers(mWindow.get());
}

bool GLRenderer::shouldClose() const {
    return glfwWindowShouldClose(mWindow.get());
}

void GLRenderer::close() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    mWindow.reset();
    glfwTerminate();
}

void GLRenderer::initializeGLFW() {
    if (glfwInit() == GLFW_FALSE) {
        throw RendererError("Failed to initialize GLFW");
    }
}

void GLRenderer::initializeImGuiBackend() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (!ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true)) {
        throw RendererError("Failed to initialize ImGui GLFW backend");
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330")) {
        ImGui_ImplGlfw_Shutdown();
        throw RendererError("Failed to initialize ImGui OpenGL3 backend");
    }
}

void GLRenderer::openWindow(std::string_view title, IVec2 size) {
    // Configure GLFW for OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto *window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw RendererError("Failed to create GLFW window");
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    mWindow = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(window, glfwDestroyWindow);
}

void GLRenderer::createContext() {
    glfwMakeContextCurrent(mWindow.get());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw RendererError("Failed to initialize GLAD");
    }

    glfwSwapInterval(1);
}

void GLRenderer::sizeChanged(IVec2 size) {
    glViewport(0, 0, size.x, size.y);
}

void GLRenderer::errorCallback(int error, const char* description) {
    throw RendererError(fmt::format("GLFW Error {}: {}", error, description));
}

void GLRenderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto *renderer = static_cast<GLRenderer*>(glfwGetWindowUserPointer(window));
    if (!renderer) {
        throw RendererError("Failed to get renderer from window user pointer");
    }
    renderer->sizeChanged(IVec2(width, height));
}

} // namespace echonous