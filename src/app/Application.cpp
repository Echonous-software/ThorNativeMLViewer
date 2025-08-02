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
    mUserInterface = std::make_unique<UserInterface>();
    mDataManager = std::make_unique<DataManager>();

    mAppState.rootPath = std::filesystem::current_path();
}

Application::~Application() {}

int Application::run() {

    auto samplePath = mAppState.rootPath / "data" / "samples" / "8S138JEBXKUX4D0N_9_224.bin";
    auto sampleImageSize = ImageSize{224, 224, 1, ChannelOrder::CHW};
    mDataManager->loadRawImageSequence(samplePath, sampleImageSize, DataType::F32);

    while (!mRenderer->shouldClose()) {
        mRenderer->beginFrame();

        mDataManager->updateState(mAppState);
        mUserInterface->draw(mAppState);

        mRenderer->render(mAppState.renderState);
        mRenderer->endFrame();
    }

    return 0;
}

} // namespace echonous