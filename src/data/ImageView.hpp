#pragma once

#include <cstdint>
#include <span>
#include <core/Math.hpp>
#include <data/DataType.hpp>
#include <data/ImageSize.hpp>

namespace echonous {

class ImageView {
    ImageSize mSize;
    DataType mDataType;

    // Invariant: exactly one of the following is non-empty
    std::span<const uint8_t> mDataU8;
    std::span<const float>   mDataF32;
public:
    ImageView(ImageSize size, std::span<const uint8_t> data);
    ImageView(ImageSize size, std::span<const float> data);

    ImageSize size() const;
    DataType dataType() const;
    std::span<const uint8_t> dataU8() const;
    std::span<const float> dataF32() const;
};

} // namespace echonous