#include <data/ImageView.hpp>
#include <cassert>

namespace echonous {

ImageView::ImageView(ImageSize size, std::span<const uint8_t> data)
    : mSize(size), mDataType(DataType::U8), mDataU8(data) {}

ImageView::ImageView(ImageSize size, std::span<const float> data)
    : mSize(size), mDataType(DataType::F32), mDataF32(data) {}

ImageSize ImageView::size() const {
    return mSize;
}

DataType ImageView::dataType() const {
    return mDataType;
}

std::span<const uint8_t> ImageView::dataU8() const {
    assert(mDataType == DataType::U8);
    return mDataU8;
}

std::span<const float> ImageView::dataF32() const {
    assert(mDataType == DataType::F32);
    return mDataF32;
}
} // namespace echonous