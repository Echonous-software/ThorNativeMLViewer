#pragma once

#include <data/ImageSequence.hpp>
#include <filesystem>
#include <memory>
#include <vector>
#include <cstdint>

namespace thor::data {

class ImageLoader {
public:
    ImageLoader() = default;
    ~ImageLoader() = default;
    
    // Disable copy (file operations are expensive)
    ImageLoader(const ImageLoader&) = delete;
    ImageLoader& operator=(const ImageLoader&) = delete;
    
    // Enable move
    ImageLoader(ImageLoader&&) = default;
    ImageLoader& operator=(ImageLoader&&) = default;
    
    // Main loading interface
    std::unique_ptr<ImageSequence> loadImageSequence(
        const std::filesystem::path& filePath,
        uint32_t width,
        uint32_t height,
        ImageDataType pixelType,
        uint32_t channels = 3,
        float fps = 30.0f
    );
    
    // Convenience methods for common formats
    std::unique_ptr<ImageSequence> loadImageSequence128(
        const std::filesystem::path& filePath,
        ImageDataType pixelType,
        uint32_t channels = 3,
        float fps = 30.0f
    );
    
    std::unique_ptr<ImageSequence> loadImageSequence224(
        const std::filesystem::path& filePath,
        ImageDataType pixelType,
        uint32_t channels = 3,
        float fps = 30.0f
    );
    
    // File validation methods
    static uint32_t calculateFrameCount(
        const std::filesystem::path& filePath,
        uint32_t width,
        uint32_t height,
        uint32_t channels,
        ImageDataType pixelType
    );
    
private:
    // Byte ordering conversion methods (little-endian default)
    static uint32_t extractLittleEndian32(const uint8_t* data);
    static float uint32ToFloat(uint32_t value);
    
    // File reading methods
    std::vector<uint8_t> readBinaryFile(const std::filesystem::path& filePath);
    void validateFilePath(const std::filesystem::path& filePath);
    void validateDimensions(uint32_t width, uint32_t height, uint32_t channels);
    
    // Frame loading methods
    void loadUint8Frames(const std::vector<uint8_t>& fileData, ImageSequence& sequence, uint32_t frameCount);
    void loadFloat32Frames(const std::vector<uint8_t>& fileData, ImageSequence& sequence, uint32_t frameCount);
    
    // Helper methods
    static size_t calculateFrameSize(uint32_t width, uint32_t height, uint32_t channels, ImageDataType pixelType);
    static size_t calculatePixelSize(ImageDataType pixelType);
};

} // namespace thor::data 