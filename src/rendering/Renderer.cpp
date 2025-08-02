#include <rendering/Renderer.hpp>
#include <rendering/opengl/GLRenderer.hpp>

namespace echonous {

std::unique_ptr<Renderer> GetPlatformRenderer(std::string_view name, IVec2 windowSize) {
    return std::make_unique<GLRenderer>(name, windowSize);
}

} // namespace echonous