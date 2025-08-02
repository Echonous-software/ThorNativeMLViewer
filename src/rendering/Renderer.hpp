#pragma once

#include <string_view>
#include <memory>
#include <core/Error.hpp>
#include <core/Math.hpp>
#include <app/AppState.hpp>

namespace echonous {

class RendererError : public Error {
public:
    RendererError(const char* message) : Error(message) {}
    RendererError(const std::string& message) : Error(message) {}
};

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual std::string_view name() const = 0;

    virtual void beginFrame() = 0;
    virtual void render(const RenderState& state) = 0;
    virtual void endFrame() = 0;
    virtual bool shouldClose() const = 0;
    virtual void close() = 0;
};

std::unique_ptr<Renderer> GetPlatformRenderer(std::string_view name, IVec2 windowSize);

} // namespace echonous