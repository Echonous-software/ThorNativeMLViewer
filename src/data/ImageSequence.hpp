#pragma once

#include <cstdint>
#include <vector>
#include <data/DataType.hpp>
#include <data/ImageView.hpp>
#include <data/ImageSize.hpp>

namespace echonous {


class ImageSequence {
    int mFrameCount;
    float mFPS;
    DataType mDataType;
    ImageSize mImageSize;
    float mMinValue;
    float mMaxValue;

    // Invariant: exactly one of the following is non-empty, matching mDataType
    std::vector<uint8_t> mDataU8;
    std::vector<float>  mDataF32;
public:
    ImageSequence(std::vector<uint8_t> &&data, ImageSize size, float fps=30.0f);
    ImageSequence(std::vector<float> &&data,   ImageSize size, float fps=30.0f);

    int frameCount() const;
    float fps() const;
    DataType dataType() const;
    ImageSize imageSize() const;
    float minValue() const;
    float maxValue() const;
    int numBytes() const;

    ImageView frame(int index) const;
};

} // namespace echonous