#include <app/Application.hpp>
#include <core/Error.hpp>
#include <rendering/Renderer.hpp>
#include <ui/UserInterface.hpp>
#include <app/PlaybackController.hpp>
#include <data/DataManager.hpp>

namespace echonous {

Application::Application(std::string_view name, std::string_view package)
    : mName(name), mPackage(package)
{
    mRenderer = GetPlatformRenderer(name, IVec2(1280, 720));
}

Application::~Application() {}

int Application::run() {
    while (!mRenderer->shouldClose()) {
        mRenderer->beginFrame();
        mRenderer->render(mAppState.renderState);
        mRenderer->endFrame();
    }

    return 0;
}

} // namespace echonous