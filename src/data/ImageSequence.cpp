#include <data/ImageSequence.hpp>
#include <core/Error.hpp>
#include <cstring>

namespace thor::data {

// ImageView Implementation

ImageView::ImageView(std::span<const uint8_t> data, uint32_t width, uint32_t height, 
                     uint32_t channels, size_t stride)
    : mDataUint8(data)
    , mDataFloat32()
    , mWidth(width)
    , mHeight(height)
    , mChannels(channels)
    , mStride(stride == 0 ? width * channels : stride)
    , mPixelType(ImageDataType::UINT8) {
    
    if (width == 0 || height == 0 || channels == 0) {
        throw core::DataFormatError("ImageView dimensions must be non-zero");
    }
    
    if (channels > 4) {
        throw core::DataFormatError("ImageView supports maximum 4 channels");
    }
    
    size_t expectedSize = static_cast<size_t>(height) * mStride;
    if (data.size() < expectedSize) {
        throw core::DataFormatError("ImageView data size too small for specified dimensions");
    }
}

ImageView::ImageView(std::span<const float> data, uint32_t width, uint32_t height,
                     uint32_t channels, size_t stride)
    : mDataUint8()
    , mDataFloat32(data)
    , mWidth(width)
    , mHeight(height)
    , mChannels(channels)
    , mStride(stride == 0 ? width * channels * sizeof(float) : stride)
    , mPixelType(ImageDataType::FLOAT32) {
    
    if (width == 0 || height == 0 || channels == 0) {
        throw core::DataFormatError("ImageView dimensions must be non-zero");
    }
    
    if (channels > 4) {
        throw core::DataFormatError("ImageView supports maximum 4 channels");
    }
    
    size_t expectedPixels = static_cast<size_t>(width) * height * channels;
    if (data.size() < expectedPixels) {
        throw core::DataFormatError("ImageView data size too small for specified dimensions");
    }
}

size_t ImageView::getDataSizeBytes() const {
    if (mPixelType == ImageDataType::UINT8) {
        return mDataUint8.size();
    } else {
        return mDataFloat32.size() * sizeof(float);
    }
}

std::span<const uint8_t> ImageView::getDataAsUint8() const {
    if (mPixelType != ImageDataType::UINT8) {
        throw core::DataFormatError("ImageView contains float32 data, not uint8");
    }
    return mDataUint8;
}

std::span<const float> ImageView::getDataAsFloat32() const {
    if (mPixelType != ImageDataType::FLOAT32) {
        throw core::DataFormatError("ImageView contains uint8 data, not float32");
    }
    return mDataFloat32;
}

const void* ImageView::getPixel(uint32_t x, uint32_t y) const {
    if (x >= mWidth || y >= mHeight) {
        throw core::DataFormatError("ImageView pixel coordinates out of bounds");
    }
    
    if (mPixelType == ImageDataType::UINT8) {
        size_t offset = y * mStride + x * mChannels;
        return mDataUint8.data() + offset;
    } else {
        size_t offset = y * mWidth * mChannels + x * mChannels;
        return mDataFloat32.data() + offset;
    }
}

// ImageSequence Implementation

ImageSequence::ImageSequence(uint32_t width, uint32_t height, uint32_t channels, 
                             ImageDataType pixelType, float fps)
    : mFrameCount(0)
    , mWidth(width)
    , mHeight(height)
    , mChannels(channels)
    , mPixelType(pixelType)
    , mFPS(fps)
    , mHasDataRange(false)
    , mDataMinValue(0.0f)
    , mDataMaxValue(0.0f) {
    
    if (width == 0 || height == 0 || channels == 0) {
        throw core::DataFormatError("ImageSequence dimensions must be non-zero");
    }
    
    if (channels > 4) {
        throw core::DataFormatError("ImageSequence supports maximum 4 channels");
    }
    
    if (fps <= 0.0f) {
        throw core::DataFormatError("ImageSequence FPS must be positive");
    }
}

void ImageSequence::addFrame(const void* frameData, size_t dataSize) {
    validateFrameData(frameData, dataSize);
    
    if (mPixelType == ImageDataType::UINT8) {
        const uint8_t* typedData = static_cast<const uint8_t*>(frameData);
        mFrameDataUint8.insert(mFrameDataUint8.end(), typedData, typedData + dataSize);
    } else {
        if (dataSize % sizeof(float) != 0) {
            throw core::DataFormatError("Float32 frame data size must be multiple of sizeof(float)");
        }
        const float* typedData = static_cast<const float*>(frameData);
        size_t floatCount = dataSize / sizeof(float);
        mFrameDataFloat32.insert(mFrameDataFloat32.end(), typedData, typedData + floatCount);
    }
    
    ++mFrameCount;
}

void ImageSequence::addFrame(std::span<const uint8_t> frameData) {
    if (mPixelType != ImageDataType::UINT8) {
        throw core::DataFormatError("Cannot add uint8 frame to float32 ImageSequence");
    }
    
    size_t expectedSize = getFrameSizeBytes();
    if (frameData.size() != expectedSize) {
        throw core::DataFormatError("Frame data size does not match expected size");
    }
    
    mFrameDataUint8.insert(mFrameDataUint8.end(), frameData.begin(), frameData.end());
    ++mFrameCount;
}

void ImageSequence::addFrame(std::span<const float> frameData) {
    if (mPixelType != ImageDataType::FLOAT32) {
        throw core::DataFormatError("Cannot add float32 frame to uint8 ImageSequence");
    }
    
    size_t expectedPixels = static_cast<size_t>(mWidth) * mHeight * mChannels;
    if (frameData.size() != expectedPixels) {
        throw core::DataFormatError("Frame data size does not match expected pixel count");
    }
    
    mFrameDataFloat32.insert(mFrameDataFloat32.end(), frameData.begin(), frameData.end());
    ++mFrameCount;
}

void ImageSequence::reserveFrames(uint32_t frameCount) {
    size_t totalSize = static_cast<size_t>(frameCount) * getFrameSizeBytes();
    
    if (mPixelType == ImageDataType::UINT8) {
        mFrameDataUint8.reserve(totalSize);
    } else {
        size_t floatCount = totalSize / sizeof(float);
        mFrameDataFloat32.reserve(floatCount);
    }
}

void ImageSequence::clear() {
    mFrameDataUint8.clear();
    mFrameDataFloat32.clear();
    mFrameCount = 0;
}

ImageView ImageSequence::getImageView(uint32_t frameIndex) const {
    validateFrameIndex(frameIndex);
    
    if (mPixelType == ImageDataType::UINT8) {
        auto frameData = getFrameDataUint8(frameIndex);
        return ImageView(frameData, mWidth, mHeight, mChannels);
    } else {
        auto frameData = getFrameDataFloat32(frameIndex);
        return ImageView(frameData, mWidth, mHeight, mChannels);
    }
}

size_t ImageSequence::getFrameSizeBytes() const {
    return static_cast<size_t>(mWidth) * mHeight * mChannels * getPixelSizeBytes();
}

size_t ImageSequence::getTotalSizeBytes() const {
    return static_cast<size_t>(mFrameCount) * getFrameSizeBytes();
}

// Private helper methods

size_t ImageSequence::getPixelSizeBytes() const {
    return (mPixelType == ImageDataType::UINT8) ? sizeof(uint8_t) : sizeof(float);
}

void ImageSequence::validateFrameIndex(uint32_t frameIndex) const {
    if (frameIndex >= mFrameCount) {
        throw core::DataFormatError("Frame index out of bounds");
    }
}

void ImageSequence::validateFrameData(const void* frameData, size_t dataSize) const {
    if (frameData == nullptr) {
        throw core::DataFormatError("Frame data cannot be null");
    }
    
    size_t expectedSize = getFrameSizeBytes();
    if (dataSize != expectedSize) {
        throw core::DataFormatError("Frame data size does not match expected size");
    }
}

std::span<const uint8_t> ImageSequence::getFrameDataUint8(uint32_t frameIndex) const {
    if (mPixelType != ImageDataType::UINT8) {
        throw core::DataFormatError("Cannot get uint8 data from float32 ImageSequence");
    }
    
    size_t frameSize = getFrameSizeBytes();
    size_t startOffset = static_cast<size_t>(frameIndex) * frameSize;
    
    return std::span<const uint8_t>(
        mFrameDataUint8.data() + startOffset, 
        frameSize
    );
}

std::span<const float> ImageSequence::getFrameDataFloat32(uint32_t frameIndex) const {
    if (mPixelType != ImageDataType::FLOAT32) {
        throw core::DataFormatError("Cannot get float32 data from uint8 ImageSequence");
    }
    
    size_t pixelsPerFrame = static_cast<size_t>(mWidth) * mHeight * mChannels;
    size_t startOffset = static_cast<size_t>(frameIndex) * pixelsPerFrame;
    
    return std::span<const float>(
        mFrameDataFloat32.data() + startOffset,
        pixelsPerFrame
    );
}

} // namespace thor::data 