#pragma once

#include <filesystem>
#include <optional>
#include <data/ImageSequence.hpp>

namespace echonous {

struct RenderState {
    std::optional<std::filesystem::path> currentFilePath = std::nullopt;
    std::optional<ImageSequence*> currentImageSequence = std::nullopt;
    int previousFrameIndex = 0;
    int currentFrameIndex = 0;
};

struct AppState {
    RenderState renderState;
    std::filesystem::path rootPath;
};

} // namespace echonous