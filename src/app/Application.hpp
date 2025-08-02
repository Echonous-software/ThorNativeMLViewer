#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <app/AppState.hpp>

namespace echonous {

class Renderer;
class UserInterface;
class PlaybackController;
class DataManager;

class Application {
   
    std::string mName;
    std::string mPackage;

    std::unique_ptr<Renderer> mRenderer;
    std::unique_ptr<UserInterface> mUserInterface;
    std::unique_ptr<PlaybackController> mPlaybackController;
    std::unique_ptr<DataManager> mDataManager;

    AppState mAppState;

public:
    Application(std::string_view name, std::string_view package);
    ~Application();
    int run();

private:
};

}