#include <data/ImageSize.hpp>
#include <fmt/format.h>

namespace echonous {

std::string format_as(ImageSize size) {
    if (size.channelOrder == ChannelOrder::CHW) {
        return fmt::format("{}x{}x{} (CHW)", size.channels, size.height, size.width);
    }
    else if (size.channelOrder == ChannelOrder::HWC) {
        return fmt::format("{}x{}x{} (HWC)", size.height, size.width, size.channels);
    }
    // unreachable
    __builtin_unreachable();
}

} // namespace echonous