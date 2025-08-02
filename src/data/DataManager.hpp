#pragma once

#include <memory>
#include <filesystem>
#include <core/Math.hpp>
#include <core/Error.hpp>

namespace echonous {

class ImageSequence;

enum class DataType {
    U8,
    F32
};

class DataError : public Error {
public:
    DataError(const char* message) : Error(message) {}
    DataError(const std::string& message) : Error(message) {}
};

class DataReadError : public DataError {
public:
    DataReadError(const char* message) : DataError(message) {}
    DataReadError(const std::string& message) : DataError(message) {}
};

class DataManager {
    /* nullable, current image sequence */
    std::unique_ptr<ImageSequence> mImageSequence;
public:
    ~DataManager();

    void loadRawImageSequence(std::filesystem::path path, IVec2 size, DataType dataType);
    std::optional<ImageSequence*> currentImageSequence() const;
};

} // namespace echonous