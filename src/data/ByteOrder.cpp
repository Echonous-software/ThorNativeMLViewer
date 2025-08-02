#include <data/ByteOrder.hpp>
#include <cassert>
#include <cstring>

namespace echonous {

uint16_t ReadUInt16LE(std::span<const uint8_t> data) {
    assert(data.size() == 2);
    return static_cast<uint16_t>(data[1]) << 8 | static_cast<uint16_t>(data[0]);
}

uint16_t ReadUInt16BE(std::span<const uint8_t> data) {
    assert(data.size() == 2);
    return static_cast<uint16_t>(data[0]) << 8 | static_cast<uint16_t>(data[1]);
}

uint32_t ReadUInt32LE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    return static_cast<uint32_t>(data[3]) << 24 | static_cast<uint32_t>(data[2]) << 16 | static_cast<uint32_t>(data[1]) << 8 | static_cast<uint32_t>(data[0]);
}

uint32_t ReadUInt32BE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    return static_cast<uint32_t>(data[0]) << 24 | static_cast<uint32_t>(data[1]) << 16 | static_cast<uint32_t>(data[2]) << 8 | static_cast<uint32_t>(data[3]);
}

uint64_t ReadUInt64LE(std::span<const uint8_t> data) {
    assert(data.size() == 8);
    return static_cast<uint64_t>(data[7]) << 56 | static_cast<uint64_t>(data[6]) << 48 | static_cast<uint64_t>(data[5]) << 40 | static_cast<uint64_t>(data[4]) << 32 | static_cast<uint64_t>(data[3]) << 24 | static_cast<uint64_t>(data[2]) << 16 | static_cast<uint64_t>(data[1]) << 8 | static_cast<uint64_t>(data[0]);
}

uint64_t ReadUInt64BE(std::span<const uint8_t> data) {
    assert(data.size() == 8);
    return static_cast<uint64_t>(data[0]) << 56 | static_cast<uint64_t>(data[1]) << 48 | static_cast<uint64_t>(data[2]) << 40 | static_cast<uint64_t>(data[3]) << 32 | static_cast<uint64_t>(data[4]) << 24 | static_cast<uint64_t>(data[5]) << 16 | static_cast<uint64_t>(data[6]) << 8 | static_cast<uint64_t>(data[7]);
}

int16_t ReadInt16LE(std::span<const uint8_t> data) {
    assert(data.size() == 2);
    return static_cast<int16_t>(data[1]) << 8 | static_cast<int16_t>(data[0]);
}

int16_t ReadInt16BE(std::span<const uint8_t> data) {
    assert(data.size() == 2);
    return static_cast<int16_t>(data[0]) << 8 | static_cast<int16_t>(data[1]);
}

int32_t ReadInt32LE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    return static_cast<int32_t>(data[3]) << 24 | static_cast<int32_t>(data[2]) << 16 | static_cast<int32_t>(data[1]) << 8 | static_cast<int32_t>(data[0]);
}

int32_t ReadInt32BE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    return static_cast<int32_t>(data[0]) << 24 | static_cast<int32_t>(data[1]) << 16 | static_cast<int32_t>(data[2]) << 8 | static_cast<int32_t>(data[3]);
}

int64_t ReadInt64LE(std::span<const uint8_t> data) {    
    assert(data.size() == 8);
    return static_cast<int64_t>(data[7]) << 56 | static_cast<int64_t>(data[6]) << 48 | static_cast<int64_t>(data[5]) << 40 | static_cast<int64_t>(data[4]) << 32 | static_cast<int64_t>(data[3]) << 24 | static_cast<int64_t>(data[2]) << 16 | static_cast<int64_t>(data[1]) << 8 | static_cast<int64_t>(data[0]);
}

int64_t ReadInt64BE(std::span<const uint8_t> data) {
    assert(data.size() == 8);
    return static_cast<int64_t>(data[0]) << 56 | static_cast<int64_t>(data[1]) << 48 | static_cast<int64_t>(data[2]) << 40 | static_cast<int64_t>(data[3]) << 32 | static_cast<int64_t>(data[4]) << 24 | static_cast<int64_t>(data[5]) << 16 | static_cast<int64_t>(data[6]) << 8 | static_cast<int64_t>(data[7]);
}

float ReadFloatLE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    float value;
    uint32_t bytes = ReadUInt32LE(data);
    std::memcpy(&value, &bytes, sizeof(float));
    return value;
}

float ReadFloatBE(std::span<const uint8_t> data) {
    assert(data.size() == 4);
    float value;
    uint32_t bytes = ReadUInt32BE(data);
    std::memcpy(&value, &bytes, sizeof(float));
    return value;
}

double ReadDoubleLE(std::span<const uint8_t> data) {
    assert(data.size() == 8);
    double value;
    uint64_t bytes = ReadUInt64LE(data);
    std::memcpy(&value, &bytes, sizeof(double));
    return value;
}

double ReadDoubleBE(std::span<const uint8_t> data) {
    assert(data.size() == 8);
    double value;
    uint64_t bytes = ReadUInt64BE(data);
    std::memcpy(&value, &bytes, sizeof(double));
    return value;
}

} // namespace echonous