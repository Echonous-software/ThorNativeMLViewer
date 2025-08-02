#pragma once

#include <fmt/format.h>

namespace echonous {

enum class DataType {
    U8,
    F32
};

fmt::string_view format_as(DataType type);

} // namespace echonous