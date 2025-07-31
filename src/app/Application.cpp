#include <app/Application.hpp>
#include <rendering/GLContext.hpp>
#include <rendering/GLRenderer.hpp>
#include <ui/UIManager.hpp>
#include <data/DataManager.hpp>
#include <core/Error.hpp>
#include <iostream>

namespace thor::app {

Application::Application() 
    : mGLContext(std::make_unique<thor::rendering::GLContext>())
    , mGLRenderer(std::make_unique<thor::rendering::GLRenderer>())
    , mUIManager(std::make_unique<thor::ui::UIManager>())
    , mDataManager(std::make_unique<thor::data::DataManager>())
    , mInitialized(false)
    , mCurrentTextureId(0) {
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    if (mInitialized) {
        return true;
    }
    
    try {
        // Initialize OpenGL context first
        if (!mGLContext->initialize(800, 600, "Thor Native ML Viewer")) {
            throw thor::core::InitializationError("Failed to initialize OpenGL context");
        }
        
        // Initialize other components
        if (!initializeComponents()) {
            throw thor::core::InitializationError("Failed to initialize application components");
        }
        
        // Wire up UI callbacks
        wireUICallbacks();
        
        mInitialized = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Application initialization failed: " << e.what() << std::endl;
        shutdown();
        return false;
    }
}

bool Application::initializeComponents() {
    // Initialize GLRenderer
    if (!mGLRenderer->initialize()) {
        std::cerr << "Failed to initialize GLRenderer" << std::endl;
        return false;
    }
    
    // Initialize UI manager
    if (!mUIManager->initialize(mGLContext->getWindow())) {
        std::cerr << "Failed to initialize UI manager" << std::endl;
        return false;
    }
    
    // Set up component references
    mUIManager->setDataManager(mDataManager.get());
    mUIManager->setGLRenderer(mGLRenderer.get());
    
    return true;
}

void Application::wireUICallbacks() {
    // Set up UI callbacks
    mUIManager->setPlayPauseCallback([this]() { onPlayPause(); });
    mUIManager->setNextFrameCallback([this]() { onNextFrame(); });
    mUIManager->setPreviousFrameCallback([this]() { onPreviousFrame(); });
    mUIManager->setFrameSetCallback([this](uint32_t frame) { onSetFrame(frame); });
    mUIManager->setFPSChangeCallback([this](float fps) { onFPSChange(fps); });
    mUIManager->setMinMaxChangeCallback([this](float minValue, float maxValue) { onMinMaxChange(minValue, maxValue); });
    
    // Set up playback controller callback
    auto& playbackController = mDataManager->getPlaybackController();
    playbackController.setFrameChangeCallback([this](uint32_t currentFrame, uint32_t totalFrames) {
        updateUIState();
        updateTexture();
    });
}

int Application::run() {
    if (!mInitialized && !initialize()) {
        return -1;
    }
    
    try {
        // Load a sample image sequence for demonstration if available
        std::filesystem::path samplePath = "data/samples/8S138JEBXKUX4D0N_9_224.bin";
        if (std::filesystem::exists(samplePath)) {
            std::cout << "Loading sample image sequence..." << std::endl;
            loadImageSequence224(samplePath, thor::data::ImageDataType::FLOAT32, 1);
        }
        
        // Main application loop
        while (!mGLContext->shouldClose()) {
            mGLContext->pollEvents();
            
            // Update playback controller
            auto& playbackController = mDataManager->getPlaybackController();
            playbackController.update();
            
            renderFrame();
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return -1;
    }
}

void Application::renderFrame() {
    // Clear the screen
    mGLContext->clear();
    
    // Render texture if available
    if (mCurrentTextureId != 0 && mDataManager->hasSequence()) {
        auto imageView = mDataManager->getCurrentImageView();
        if (imageView.has_value()) {
            auto& params = mGLRenderer->getRenderingParameters();
            uint32_t channels = imageView->getChannels();
            mGLRenderer->renderTexturedQuad(mCurrentTextureId, channels, params);
        }
    }
    
    // Start ImGui frame
    mUIManager->newFrame();
    
    // Update UI state
    updateUIState();
    
    // Render ImGui
    mUIManager->render();
    
    // Swap buffers
    mGLContext->swapBuffers();
}

void Application::updateTexture() {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto imageView = mDataManager->getCurrentImageView();
    if (!imageView.has_value()) {
        return;
    }
    
    try {
        // Create or update texture
        if (mCurrentTextureId == 0) {
            mCurrentTextureId = mGLRenderer->createTexture(imageView.value());
        } else {
            mGLRenderer->updateTexture(mCurrentTextureId, imageView.value());
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to update texture: " << e.what() << std::endl;
    }
}

void Application::updateUIState() {
    if (!mDataManager->hasSequence()) {
        mUIManager->updatePlaybackState(false, 0, 0);
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    bool isPlaying = playbackController.isPlaying();
    uint32_t currentFrame = playbackController.getCurrentFrame();
    uint32_t totalFrames = mDataManager->getFrameCount();
    
    mUIManager->updatePlaybackState(isPlaying, currentFrame, totalFrames);
    
    // Update rendering parameters
    auto& params = mGLRenderer->getRenderingParameters();
    mUIManager->updateRenderingParameters(params);
}

void Application::shutdown() {
    // Clean up texture
    if (mCurrentTextureId != 0) {
        mGLRenderer->deleteTexture(mCurrentTextureId);
        mCurrentTextureId = 0;
    }
    
    // Shutdown components in reverse order
    if (mUIManager) {
        mUIManager->shutdown();
    }
    
    if (mGLRenderer) {
        mGLRenderer->shutdown();
    }
    
    if (mGLContext) {
        mGLContext->shutdown();
    }
    
    mInitialized = false;
}

// Public interface methods
bool Application::loadImageSequence(const std::filesystem::path& filePath, 
                                   uint32_t width, uint32_t height, 
                                   thor::data::ImageDataType pixelType, 
                                   uint32_t channels) {
    if (!mDataManager) {
        return false;
    }
    
    bool success = mDataManager->loadImageSequence(filePath, width, height, pixelType, channels);
    if (success) {
        // Auto-detect and set rendering parameters for float data
        if (pixelType == thor::data::ImageDataType::FLOAT32 && mDataManager->hasDataRange() && mGLRenderer) {
            float dataMin = mDataManager->getDataMinValue();
            float dataMax = mDataManager->getDataMaxValue();
            
            // Apply auto-detection logic based on detected range
            float renderingMin, renderingMax;
            
            if (dataMin >= 0.0f && dataMax <= 255.0f) {
                // Data appears to be in 0-255 range
                renderingMin = 0.0f;
                renderingMax = 255.0f;
            } else if (dataMin >= 0.0f && dataMax <= 1.0f) {
                // Data appears to be in 0-1 range
                renderingMin = 0.0f;
                renderingMax = 1.0f;
            } else {
                // Use default 0-1 range for other cases
                renderingMin = 0.0f;
                renderingMax = 1.0f;
            }
            
            // Set the auto-detected rendering parameters
            mGLRenderer->setRenderingParameters(renderingMin, renderingMax);
            
            std::cout << "Auto-detected data range: " << dataMin << " to " << dataMax 
                      << ", set rendering range: " << renderingMin << " to " << renderingMax << std::endl;
        }
        
        updateTexture();
        updateUIState();
    }
    return success;
}

bool Application::loadImageSequence128(const std::filesystem::path& filePath, 
                                      thor::data::ImageDataType pixelType, 
                                      uint32_t channels) {
    return loadImageSequence(filePath, 128, 128, pixelType, channels);
}

bool Application::loadImageSequence224(const std::filesystem::path& filePath, 
                                      thor::data::ImageDataType pixelType, 
                                      uint32_t channels) {
    return loadImageSequence(filePath, 224, 224, pixelType, channels);
}

// UI callback handlers
void Application::onPlayPause() {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    if (playbackController.isPlaying()) {
        playbackController.pause();
    } else {
        playbackController.play();
    }
}

void Application::onNextFrame() {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    playbackController.nextFrame();
}

void Application::onPreviousFrame() {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    playbackController.previousFrame();
}

void Application::onSetFrame(uint32_t frameIndex) {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    playbackController.setFrame(frameIndex);
}

void Application::onFPSChange(float fps) {
    if (!mDataManager->hasSequence()) {
        return;
    }
    
    auto& playbackController = mDataManager->getPlaybackController();
    playbackController.setFPS(fps);
}

void Application::onMinMaxChange(float minValue, float maxValue) {
    if (!mGLRenderer) {
        return;
    }
    
    mGLRenderer->setRenderingParameters(minValue, maxValue);
}

} // namespace thor::app 