#include <data/DataManager.hpp>

namespace echonous {

class ImageSequence {};

DataManager::~DataManager() {}

void DataManager::loadRawImageSequence(std::filesystem::path path, IVec2 size, DataType dataType) {
}

std::optional<ImageSequence*> DataManager::currentImageSequence() const {
    if (mImageSequence) {
        return mImageSequence.get();
    }
    return std::nullopt;
}


} // namespace echonous