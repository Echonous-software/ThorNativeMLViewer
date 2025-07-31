#pragma once

#include <data/ImageSequence.hpp>
#include <data/ImageLoader.hpp>
#include <playback/PlaybackController.hpp>
#include <filesystem>
#include <memory>
#include <optional>

namespace thor::data {

class DataManager {
public:
    DataManager();
    ~DataManager() = default;
    
    // Disable copy (manages unique resources)
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    
    // Enable move
    DataManager(DataManager&&) = default;
    DataManager& operator=(DataManager&&) = default;
    
    // Main data loading interface
    bool loadImageSequence(const std::filesystem::path& filePath,
                          uint32_t width,
                          uint32_t height,
                          ImageDataType pixelType,
                          uint32_t channels = 3);
    
    // Convenience methods for common formats
    bool loadImageSequence128(const std::filesystem::path& filePath,
                             ImageDataType pixelType,
                             uint32_t channels = 3);
    
    bool loadImageSequence224(const std::filesystem::path& filePath,
                             ImageDataType pixelType, 
                             uint32_t channels = 3);
    
    // Data access
    std::optional<ImageView> getCurrentImageView() const;
    std::optional<ImageView> getImageView(uint32_t frameIndex) const;
    
    // Sequence information
    bool hasSequence() const { return mImageSequence != nullptr; }
    uint32_t getFrameCount() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getChannels() const;
    ImageDataType getPixelType() const;
    float getFPS() const;
    
    // Data range information (for float data)
    bool hasDataRange() const;
    float getDataMinValue() const;
    float getDataMaxValue() const;
    
    // Playback controller access
    thor::playback::PlaybackController& getPlaybackController() { return mPlaybackController; }
    const thor::playback::PlaybackController& getPlaybackController() const { return mPlaybackController; }
    
    // Data management
    void clear();
    
private:
    // Components
    ImageLoader mImageLoader;
    thor::playback::PlaybackController mPlaybackController;
    
    // Current data
    std::unique_ptr<ImageSequence> mImageSequence;
    
    // Helper methods
    void setupPlaybackController();
    void onFrameChanged(uint32_t currentFrame, uint32_t totalFrames);
};

} // namespace thor::data 