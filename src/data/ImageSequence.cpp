#include <data/ImageSequence.hpp>
#include <data/DataError.hpp>
#include <algorithm>
#include <cassert>

namespace echonous {

ImageSequence::ImageSequence(std::vector<uint8_t> &&data, ImageSize size, float fps) :
    mFrameCount(0),
    mFPS(fps),
    mDataType(DataType::U8),
    mImageSize(size),
    mMinValue(0.0f),
    mMaxValue(0.0f),
    mDataU8(std::move(data))
{
    int frameSize = mImageSize.width * mImageSize.height * mImageSize.channels;
    if (mDataU8.size() % frameSize != 0) {
        throw DataReadError("Data size is not a multiple of the frame size");
    }
    mFrameCount = mDataU8.size() / frameSize;

    auto minmax = std::minmax_element(mDataU8.begin(), mDataU8.end());
    mMinValue = static_cast<float>(*minmax.first);
    mMaxValue = static_cast<float>(*minmax.second);
}

ImageSequence::ImageSequence(std::vector<float> &&data, ImageSize size, float fps) :
    mFrameCount(0),
    mFPS(fps),
    mDataType(DataType::F32),
    mImageSize(size),
    mMinValue(0.0f),
    mMaxValue(0.0f),
    mDataF32(std::move(data))
{
    int frameSize = mImageSize.width * mImageSize.height * mImageSize.channels;
    if (mDataF32.size() % frameSize != 0) {
        throw DataReadError("Data size is not a multiple of the frame size");
    }
    mFrameCount = mDataF32.size() / frameSize;

    auto minmax = std::minmax_element(mDataF32.begin(), mDataF32.end());
    mMinValue = *minmax.first;
    mMaxValue = *minmax.second;
}

int ImageSequence::frameCount() const {
    return mFrameCount;
}

float ImageSequence::fps() const {
    return mFPS;
}

DataType ImageSequence::dataType() const {
    return mDataType;
}

ImageSize ImageSequence::imageSize() const {
    return mImageSize;
}

float ImageSequence::minValue() const {
    return mMinValue;
}

float ImageSequence::maxValue() const {
    return mMaxValue;
}

int ImageSequence::numBytes() const {
    return mDataU8.size() + mDataF32.size() * sizeof(float);
}

ImageView ImageSequence::frame(int index) const {
    assert(0 <= index && index < mFrameCount);
    int frameSize = mImageSize.width * mImageSize.height * mImageSize.channels;
    int offset = index * frameSize;
    if (mDataType == DataType::U8) {
        return ImageView(mImageSize, std::span<const uint8_t>(mDataU8.data() + offset, frameSize));
    }
    else if (mDataType == DataType::F32) {
        return ImageView(mImageSize, std::span<const float>(mDataF32.data() + offset, frameSize));
    }
    // unreachable
}
} // namespace echonous