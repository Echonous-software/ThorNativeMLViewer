#pragma once

#include <cstdint>
#include <span>

namespace echonous {

uint16_t ReadUInt16LE(std::span<const uint8_t> data);
uint16_t ReadUInt16BE(std::span<const uint8_t> data);
uint32_t ReadUInt32LE(std::span<const uint8_t> data);
uint32_t ReadUInt32BE(std::span<const uint8_t> data);
uint64_t ReadUInt64LE(std::span<const uint8_t> data);
uint64_t ReadUInt64BE(std::span<const uint8_t> data);
int16_t ReadInt16LE(std::span<const uint8_t> data);
int16_t ReadInt16BE(std::span<const uint8_t> data);
int32_t ReadInt32LE(std::span<const uint8_t> data);
int32_t ReadInt32BE(std::span<const uint8_t> data);
int64_t ReadInt64LE(std::span<const uint8_t> data);
int64_t ReadInt64BE(std::span<const uint8_t> data);

float ReadFloatLE(std::span<const uint8_t> data);
float ReadFloatBE(std::span<const uint8_t> data);
double ReadDoubleLE(std::span<const uint8_t> data);
double ReadDoubleBE(std::span<const uint8_t> data);

} // namespace echonous