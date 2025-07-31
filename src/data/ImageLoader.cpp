#include <data/ImageLoader.hpp>
#include <core/Error.hpp>
#include <fstream>
#include <cstring>
#include <span>
#include <limits>
#include <cmath>
#include <algorithm>

namespace thor::data {

std::unique_ptr<ImageSequence> ImageLoader::loadImageSequence(
    const std::filesystem::path& filePath,
    uint32_t width,
    uint32_t height,
    ImageDataType pixelType,
    uint32_t channels,
    float fps) {
    
    // Validate inputs
    validateFilePath(filePath);
    validateDimensions(width, height, channels);
    
    // Calculate expected frame count from file size
    uint32_t frameCount = calculateFrameCount(filePath, width, height, channels, pixelType);
    
    if (frameCount == 0) {
        throw thor::core::DataFormatError("File is too small to contain any complete frames");
    }
    
    // Read binary file data
    std::vector<uint8_t> fileData = readBinaryFile(filePath);
    
    // Create ImageSequence
    auto sequence = std::make_unique<ImageSequence>(width, height, channels, pixelType, fps);
    sequence->reserveFrames(frameCount);
    
    // Convert and load frames based on pixel type
    if (pixelType == ImageDataType::UINT8) {
        loadUint8Frames(fileData, *sequence, frameCount);
    } else {
        loadFloat32Frames(fileData, *sequence, frameCount);
    }
    
    return sequence;
}

std::unique_ptr<ImageSequence> ImageLoader::loadImageSequence128(
    const std::filesystem::path& filePath,
    ImageDataType pixelType,
    uint32_t channels,
    float fps) {
    
    return loadImageSequence(filePath, 128, 128, pixelType, channels, fps);
}

std::unique_ptr<ImageSequence> ImageLoader::loadImageSequence224(
    const std::filesystem::path& filePath,
    ImageDataType pixelType,
    uint32_t channels,
    float fps) {
    
    return loadImageSequence(filePath, 224, 224, pixelType, channels, fps);
}

uint32_t ImageLoader::calculateFrameCount(
    const std::filesystem::path& filePath,
    uint32_t width,
    uint32_t height,
    uint32_t channels,
    ImageDataType pixelType) {
    
    if (!std::filesystem::exists(filePath)) {
        throw core::DataFormatError("File does not exist: " + filePath.string());
    }
    
    auto fileSize = std::filesystem::file_size(filePath);
    size_t frameSize = calculateFrameSize(width, height, channels, pixelType);
    
    if (fileSize % frameSize != 0) {
        throw core::DataFormatError("File size is not a multiple of frame size");
    }
    
    return static_cast<uint32_t>(fileSize / frameSize);
}

// Private implementation methods

uint32_t ImageLoader::extractLittleEndian32(const uint8_t* data) {
    return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

float ImageLoader::uint32ToFloat(uint32_t value) {
    float result;
    std::memcpy(&result, &value, sizeof(float));
    return result;
}

std::vector<uint8_t> ImageLoader::readBinaryFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        throw core::DataFormatError("Failed to open file: " + filePath.string());
    }
    
    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(fileSize);
    
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw core::DataFormatError("Failed to read file: " + filePath.string());
    }
    
    return buffer;
}

void ImageLoader::validateFilePath(const std::filesystem::path& filePath) {
    if (filePath.empty()) {
        throw core::DataFormatError("File path cannot be empty");
    }
    
    if (!std::filesystem::exists(filePath)) {
        throw core::DataFormatError("File does not exist: " + filePath.string());
    }
    
    if (!std::filesystem::is_regular_file(filePath)) {
        throw core::DataFormatError("Path is not a regular file: " + filePath.string());
    }
    
    if (std::filesystem::file_size(filePath) == 0) {
        throw core::DataFormatError("File is empty: " + filePath.string());
    }
}

void ImageLoader::validateDimensions(uint32_t width, uint32_t height, uint32_t channels) {
    if (width == 0 || height == 0) {
        throw core::DataFormatError("Image dimensions must be non-zero");
    }
    
    if (channels == 0 || channels > 4) {
        throw core::DataFormatError("Channel count must be between 1 and 4");
    }
    
    // Validate common supported dimensions
    if ((width != 128 && width != 224) || (height != 128 && height != 224)) {
        // Note: Allow other dimensions but warn about testing
        // This is flexible for future extensions
    }
}

void ImageLoader::loadUint8Frames(
    const std::vector<uint8_t>& fileData,
    ImageSequence& sequence,
    uint32_t frameCount) {
    
    size_t frameSize = calculateFrameSize(
        sequence.getWidth(),
        sequence.getHeight(), 
        sequence.getChannels(),
        sequence.getPixelType()
    );
    
    for (uint32_t frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        size_t offset = frameIndex * frameSize;
        
        if (offset + frameSize > fileData.size()) {
            throw core::DataFormatError("Insufficient data for frame " + std::to_string(frameIndex));
        }
        
        // For uint8 data, copy directly (already in correct format)
        std::span<const uint8_t> frameData(fileData.data() + offset, frameSize);
        sequence.addFrame(frameData);
    }
}

void ImageLoader::loadFloat32Frames(
    const std::vector<uint8_t>& fileData,
    ImageSequence& sequence,
    uint32_t frameCount) {
    
    size_t frameSize = calculateFrameSize(
        sequence.getWidth(),
        sequence.getHeight(),
        sequence.getChannels(),
        sequence.getPixelType()
    );
    
    size_t pixelsPerFrame = static_cast<size_t>(sequence.getWidth()) * 
                           sequence.getHeight() * 
                           sequence.getChannels();
    
    // Track min/max for auto-detection
    float globalMin = std::numeric_limits<float>::max();
    float globalMax = std::numeric_limits<float>::lowest();
    
    for (uint32_t frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
        size_t byteOffset = frameIndex * frameSize;
        
        if (byteOffset + frameSize > fileData.size()) {
            throw core::DataFormatError("Insufficient data for frame " + std::to_string(frameIndex));
        }
        
        // Convert bytes to float32 using little-endian extraction
        std::vector<float> frameFloats;
        frameFloats.reserve(pixelsPerFrame);
        
        for (size_t pixelIndex = 0; pixelIndex < pixelsPerFrame; ++pixelIndex) {
            size_t floatByteOffset = byteOffset + (pixelIndex * sizeof(float));
            
            if (floatByteOffset + sizeof(float) > fileData.size()) {
                throw core::DataFormatError("Insufficient data for pixel in frame " + std::to_string(frameIndex));
            }
            
            // Extract 32-bit value using little-endian byte ordering
            uint32_t rawValue = extractLittleEndian32(fileData.data() + floatByteOffset);
            float floatValue = uint32ToFloat(rawValue);
            
            // Track global min/max for auto-detection
            if (std::isfinite(floatValue)) { // Skip NaN and infinity values
                globalMin = std::min(globalMin, floatValue);
                globalMax = std::max(globalMax, floatValue);
            }
            
            frameFloats.push_back(floatValue);
        }
        
        // Add frame to sequence
        std::span<const float> frameData(frameFloats);
        sequence.addFrame(frameData);
    }
    
    // Store the detected data range for later use by auto-detection logic
    if (std::isfinite(globalMin) && std::isfinite(globalMax)) {
        sequence.setDataRange(globalMin, globalMax);
    }
}

size_t ImageLoader::calculateFrameSize(uint32_t width, uint32_t height, uint32_t channels, ImageDataType pixelType) {
    return static_cast<size_t>(width) * height * channels * calculatePixelSize(pixelType);
}

size_t ImageLoader::calculatePixelSize(ImageDataType pixelType) {
    switch (pixelType) {
        case ImageDataType::UINT8:
            return sizeof(uint8_t);
        case ImageDataType::FLOAT32:
            return sizeof(float);
        default:
            throw core::DataFormatError("Unsupported pixel type");
    }
}

} // namespace thor::data 