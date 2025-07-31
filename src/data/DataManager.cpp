#include <data/DataManager.hpp>
#include <core/Error.hpp>
#include <iostream>

namespace thor::data {

DataManager::DataManager()
    : mImageLoader()
    , mPlaybackController()
    , mImageSequence(nullptr) {
    
    setupPlaybackController();
}

bool DataManager::loadImageSequence(const std::filesystem::path& filePath,
                                   uint32_t width,
                                   uint32_t height,
                                   ImageDataType pixelType,
                                   uint32_t channels) {
    try {
        // Load image sequence using ImageLoader
        auto sequence = mImageLoader.loadImageSequence(filePath, width, height, pixelType, channels);
        
        if (!sequence) {
            std::cerr << "Failed to load image sequence from: " << filePath << std::endl;
            return false;
        }
        
        // Store the sequence
        mImageSequence = std::move(sequence);
        
        // Configure playback controller
        mPlaybackController.setFrameCount(mImageSequence->getFrameCount());
        mPlaybackController.setFPS(mImageSequence->getFPS());
        mPlaybackController.reset();
        
        std::cout << "Loaded image sequence: " << mImageSequence->getFrameCount() 
                  << " frames, " << width << "x" << height 
                  << " pixels, " << channels << " channels" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading image sequence: " << e.what() << std::endl;
        mImageSequence.reset();
        return false;
    }
}

bool DataManager::loadImageSequence128(const std::filesystem::path& filePath,
                                      ImageDataType pixelType,
                                      uint32_t channels) {
    return loadImageSequence(filePath, 128, 128, pixelType, channels);
}

bool DataManager::loadImageSequence224(const std::filesystem::path& filePath,
                                      ImageDataType pixelType,
                                      uint32_t channels) {
    return loadImageSequence(filePath, 224, 224, pixelType, channels);
}

std::optional<ImageView> DataManager::getCurrentImageView() const {
    if (!mImageSequence) {
        return std::nullopt;
    }
    
    try {
        uint32_t currentFrame = mPlaybackController.getCurrentFrame();
        return mImageSequence->getImageView(currentFrame);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting current image view: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::optional<ImageView> DataManager::getImageView(uint32_t frameIndex) const {
    if (!mImageSequence) {
        return std::nullopt;
    }
    
    try {
        return mImageSequence->getImageView(frameIndex);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting image view for frame " << frameIndex 
                  << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

uint32_t DataManager::getFrameCount() const {
    return mImageSequence ? mImageSequence->getFrameCount() : 0;
}

uint32_t DataManager::getWidth() const {
    return mImageSequence ? mImageSequence->getWidth() : 0;
}

uint32_t DataManager::getHeight() const {
    return mImageSequence ? mImageSequence->getHeight() : 0;
}

uint32_t DataManager::getChannels() const {
    return mImageSequence ? mImageSequence->getChannels() : 0;
}

ImageDataType DataManager::getPixelType() const {
    return mImageSequence ? mImageSequence->getPixelType() : ImageDataType::UINT8;
}

float DataManager::getFPS() const {
    return mImageSequence ? mImageSequence->getFPS() : 30.0f;
}

bool DataManager::hasDataRange() const {
    return mImageSequence ? mImageSequence->hasDataRange() : false;
}

float DataManager::getDataMinValue() const {
    return mImageSequence ? mImageSequence->getDataMinValue() : 0.0f;
}

float DataManager::getDataMaxValue() const {
    return mImageSequence ? mImageSequence->getDataMaxValue() : 0.0f;
}

void DataManager::clear() {
    mImageSequence.reset();
    mPlaybackController.reset();
}

void DataManager::setupPlaybackController() {
    // Set up frame change callback to track current frame
    auto callback = [this](uint32_t currentFrame, uint32_t totalFrames) {
        onFrameChanged(currentFrame, totalFrames);
    };
    
    mPlaybackController.setFrameChangeCallback(callback);
    mPlaybackController.setLooping(true); // Enable looping by default
}

void DataManager::onFrameChanged(uint32_t currentFrame, uint32_t totalFrames) {
    // This callback can be used for future features like:
    // - Notifying UI of frame changes
    // - Triggering ML inference on the current frame
    // - Updating visualization
    
    // For now, just validate the frame is in bounds
    if (mImageSequence && currentFrame >= mImageSequence->getFrameCount()) {
        std::cerr << "Warning: Frame index " << currentFrame 
                  << " is out of bounds (max: " << mImageSequence->getFrameCount() - 1 
                  << ")" << std::endl;
    }
}

} // namespace thor::data 