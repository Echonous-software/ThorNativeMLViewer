#pragma once

#include <data/DataManager.hpp>
#include <core/Math.hpp>
#include <optional>

namespace echonous {

struct RenderState {
    std::optional<ImageSequence*> currentImageSequence = std::nullopt;
    int previousFrameIndex = 0;
    int currentFrameIndex = 0;
};

struct AppState {
    RenderState renderState;
};

} // namespace echonous