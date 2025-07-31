#pragma once

#include <vector>
#include <span>
#include <cstdint>

namespace thor::data {

// Pixel data type enumeration
enum class ImageDataType {
    UINT8,
    FLOAT32
};

// Non-owning view into image data for zero-copy operations
class ImageView {
public:
    // Constructor for uint8 data
    ImageView(std::span<const uint8_t> data, uint32_t width, uint32_t height, 
              uint32_t channels, size_t stride = 0);
    
    // Constructor for float32 data  
    ImageView(std::span<const float> data, uint32_t width, uint32_t height,
              uint32_t channels, size_t stride = 0);
    
    // Accessors
    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }
    uint32_t getChannels() const { return mChannels; }
    size_t getStride() const { return mStride; }
    ImageDataType getPixelType() const { return mPixelType; }
    size_t getDataSizeBytes() const;
    
    // Data access
    std::span<const uint8_t> getDataAsUint8() const;
    std::span<const float> getDataAsFloat32() const;
    
    // Pixel access (bounds-checked)
    const void* getPixel(uint32_t x, uint32_t y) const;
    
private:
    std::span<const uint8_t> mDataUint8;
    std::span<const float> mDataFloat32;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mChannels;
    size_t mStride;
    ImageDataType mPixelType;
};

// Owns multi-frame image data with metadata
class ImageSequence {
public:
    // Constructor
    ImageSequence(uint32_t width, uint32_t height, uint32_t channels, 
                  ImageDataType pixelType, float fps = 30.0f);
    
    // Disable copy constructor and assignment (expensive)
    ImageSequence(const ImageSequence&) = delete;
    ImageSequence& operator=(const ImageSequence&) = delete;
    
    // Enable move semantics
    ImageSequence(ImageSequence&&) = default;
    ImageSequence& operator=(ImageSequence&&) = default;
    
    // Frame management
    void addFrame(const void* frameData, size_t dataSize);
    void addFrame(std::span<const uint8_t> frameData);
    void addFrame(std::span<const float> frameData);
    void reserveFrames(uint32_t frameCount);
    void clear();
    
    // Frame access (zero-copy)
    ImageView getImageView(uint32_t frameIndex) const;
    
    // Metadata accessors
    uint32_t getFrameCount() const { return mFrameCount; }
    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeight() const { return mHeight; }
    uint32_t getChannels() const { return mChannels; }
    ImageDataType getPixelType() const { return mPixelType; }
    float getFPS() const { return mFPS; }
    size_t getFrameSizeBytes() const;
    size_t getTotalSizeBytes() const;
    
    // Data range accessors (for float data only)
    bool hasDataRange() const { return mHasDataRange; }
    float getDataMinValue() const { return mDataMinValue; }
    float getDataMaxValue() const { return mDataMaxValue; }
    
    // Metadata setters
    void setFPS(float fps) { mFPS = fps; }
    void setDataRange(float minValue, float maxValue) { 
        mDataMinValue = minValue; 
        mDataMaxValue = maxValue; 
        mHasDataRange = true; 
    }
    
private:
    // Frame data storage - using variant approach for type safety
    std::vector<uint8_t> mFrameDataUint8;
    std::vector<float> mFrameDataFloat32;
    
    // Metadata
    uint32_t mFrameCount;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mChannels;
    ImageDataType mPixelType;
    float mFPS;
    
    // Data range (for float data only)
    bool mHasDataRange;
    float mDataMinValue;
    float mDataMaxValue;
    
    // Helper methods
    size_t getPixelSizeBytes() const;
    void validateFrameIndex(uint32_t frameIndex) const;
    void validateFrameData(const void* frameData, size_t dataSize) const;
    std::span<const uint8_t> getFrameDataUint8(uint32_t frameIndex) const;
    std::span<const float> getFrameDataFloat32(uint32_t frameIndex) const;
};

} // namespace thor::data 