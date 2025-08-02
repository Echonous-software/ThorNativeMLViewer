#pragma once

#include <app/AppState.hpp>

namespace echonous {

class UserInterface {
public:
    UserInterface();
    void draw(AppState& state);

private:
    void drawMenuBar();
    void drawSidebar(AppState& state);
    void drawCurrentFileInfo(AppState& state);
};

} // namespace echonous