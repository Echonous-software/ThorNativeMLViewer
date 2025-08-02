#include <data/DataType.hpp>

namespace echonous {

fmt::string_view format_as(DataType type) {
    switch (type) {
        case DataType::U8:
            return "U8";
        case DataType::F32:
            return "F32";
    }
    // unreachable
}

} // namespace echonous