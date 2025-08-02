#include <data/DataManager.hpp>
#include <data/DataError.hpp>
#include <data/ImageSequence.hpp>
#include <data/ByteOrder.hpp>
#include <fstream>
#include <fmt/format.h>
#include <fmt/std.h>

namespace echonous {

DataManager::DataManager() :
    mCurrentFilePath(std::nullopt) {}

DataManager::~DataManager() {}

void DataManager::loadRawImageSequence(std::filesystem::path path, ImageSize size, DataType dataType) {
    mCurrentFilePath = path;
    // Read entire file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw DataReadError(fmt::format("Failed to open file {}", path));
    }
    std::vector<uint8_t> data(std::istreambuf_iterator<char>(file), {});
    file.close();

    if (dataType == DataType::U8) {
        mImageSequence = std::make_unique<ImageSequence>(std::move(data), size);
    }
    else if (dataType == DataType::F32) {
        if (data.size() % sizeof(float) != 0) {
            throw DataReadError("Data size is not a multiple of the float size");
        }
        std::vector<float> floatData(data.size() / sizeof(float));
        for (size_t i = 0; i < floatData.size(); i++) {
            floatData[i] = ReadFloatLE(std::span<const uint8_t>(data.data() + i * sizeof(float), sizeof(float)));
        }
        mImageSequence = std::make_unique<ImageSequence>(std::move(floatData), size);
    }
    else {
        throw DataReadError(fmt::format("Unsupported data type: {}", dataType));
    }
}

void DataManager::updateState(AppState& state) {
    state.renderState.currentFilePath = mCurrentFilePath;
    if (mImageSequence) {
        state.renderState.currentImageSequence = mImageSequence.get();
    }
    else {
        state.renderState.currentImageSequence = std::nullopt;
    }
}


} // namespace echonous