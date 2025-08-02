#pragma once

#include <memory>
#include <filesystem>
#include <optional>
#include <app/AppState.hpp>
#include <core/Math.hpp>
#include <data/DataType.hpp>
#include <data/ImageSize.hpp>

namespace echonous {

class ImageSequence;
class ImageView;
class DataManager {

    std::optional<std::filesystem::path> mCurrentFilePath;

    /* nullable, current image sequence */
    std::unique_ptr<ImageSequence> mImageSequence;
public:
    DataManager();
    ~DataManager();

    void loadRawImageSequence(std::filesystem::path path, ImageSize size, DataType dataType);
    void closeImageSequence();
    void updateState(AppState& state);
};

} // namespace echonous