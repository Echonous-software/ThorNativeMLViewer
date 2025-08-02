#pragma once

#include <string>

namespace echonous {

enum class ChannelOrder {
    CHW,
    HWC,
};


struct ImageSize {
    int width;
    int height;
    int channels = 1;
    ChannelOrder channelOrder = ChannelOrder::CHW;
};

std::string format_as(ImageSize size);

} // namespace echonous