#pragma once

#include <cstdint>

namespace thor::ui::widgets {

    bool ZoomChooser(float& zoomFactor, bool& isZoomToFit, uint32_t imageWidth, uint32_t imageHeight, uint32_t windowWidth, uint32_t windowHeight);

} // namespace thor::ui::widgets