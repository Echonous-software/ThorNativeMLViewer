#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

namespace thor::playback {

// Playback state enumeration
enum class PlaybackState {
    STOPPED,
    PLAYING,
    PAUSED
};

// Frame change callback type
using FrameChangeCallback = std::function<void(uint32_t currentFrame, uint32_t totalFrames)>;

// PlaybackController class for managing frame playback timing and state
class PlaybackController {
public:
    PlaybackController();
    ~PlaybackController() = default;
    
    // Disable copy (contains state and timing)
    PlaybackController(const PlaybackController&) = delete;
    PlaybackController& operator=(const PlaybackController&) = delete;
    
    // Enable move
    PlaybackController(PlaybackController&&) = default;
    PlaybackController& operator=(PlaybackController&&) = default;
    
    // Playback control methods
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    
    // Frame navigation
    void setFrame(uint32_t frameIndex);
    void nextFrame();
    void previousFrame();
    void setFrameCount(uint32_t totalFrames);
    
    // Frame position accessors
    uint32_t getCurrentFrame() const { return mCurrentFrame; }
    uint32_t getTotalFrames() const { return mTotalFrames; }
    
    // Playback state
    PlaybackState getState() const { return mState; }
    bool isPlaying() const { return mState == PlaybackState::PLAYING; }
    bool isPaused() const { return mState == PlaybackState::PAUSED; }
    bool isStopped() const { return mState == PlaybackState::STOPPED; }
    
    // FPS and timing control
    void setFPS(float fps);
    float getFPS() const { return mFPS; }
    
    // Looping control
    void setLooping(bool enabled) { mLooping = enabled; }
    bool isLooping() const { return mLooping; }
    
    // Frame callback registration
    void setFrameChangeCallback(FrameChangeCallback callback) { mFrameChangeCallback = callback; }
    void clearFrameChangeCallback() { mFrameChangeCallback = nullptr; }
    
    // Update method (should be called regularly, e.g. in main loop)
    void update();
    
    // Time-based queries
    std::chrono::milliseconds getFrameDuration() const;
    std::chrono::steady_clock::time_point getLastFrameTime() const { return mLastFrameTime; }
    
    // Playback statistics
    double getElapsedSeconds() const;
    uint32_t getTotalFramesPlayed() const { return mTotalFramesPlayed; }
    
    // Reset to initial state
    void reset();
    
private:
    // State variables
    PlaybackState mState;
    uint32_t mCurrentFrame;
    uint32_t mTotalFrames;
    float mFPS;
    bool mLooping;
    
    // Timing variables
    std::chrono::steady_clock::time_point mLastFrameTime;
    std::chrono::steady_clock::time_point mPlayStartTime;
    std::chrono::milliseconds mFrameDuration;
    
    // Statistics
    uint32_t mTotalFramesPlayed;
    
    // Callback for frame changes
    FrameChangeCallback mFrameChangeCallback;
    
    // Private helper methods
    void updateFrameDuration();
    void advanceFrame();
    void notifyFrameChange();
    void validateFrameIndex(uint32_t frameIndex) const;
};

} // namespace thor::playback 