#include <playback/PlaybackController.hpp>
#include <core/Error.hpp>

namespace thor::playback {

PlaybackController::PlaybackController()
    : mState(PlaybackState::STOPPED)
    , mCurrentFrame(0)
    , mTotalFrames(0)
    , mFPS(30.0f)
    , mLooping(true)
    , mLastFrameTime()
    , mPlayStartTime()
    , mFrameDuration(33) // Default ~30 FPS (33ms per frame)
    , mTotalFramesPlayed(0)
    , mFrameChangeCallback(nullptr) {
    
    updateFrameDuration();
}

// Playback control methods

void PlaybackController::play() {
    if (mTotalFrames == 0) {
        throw thor::core::DataFormatError("Cannot play: no frames available");
    }
    
    if (mState != PlaybackState::PLAYING) {
        mState = PlaybackState::PLAYING;
        mLastFrameTime = std::chrono::steady_clock::now();
        
        // Record play start time for statistics
        if (mCurrentFrame == 0) {
            mPlayStartTime = mLastFrameTime;
        }
        
        notifyFrameChange();
    }
}

void PlaybackController::pause() {
    if (mState == PlaybackState::PLAYING) {
        mState = PlaybackState::PAUSED;
        notifyFrameChange();
    }
}

void PlaybackController::stop() {
    mState = PlaybackState::STOPPED;
    mCurrentFrame = 0;
    mTotalFramesPlayed = 0;
    notifyFrameChange();
}

void PlaybackController::togglePlayPause() {
    if (mState == PlaybackState::PLAYING) {
        pause();
    } else {
        play();
    }
}

// Frame navigation

void PlaybackController::setFrame(uint32_t frameIndex) {
    validateFrameIndex(frameIndex);
    
    if (mCurrentFrame != frameIndex) {
        mCurrentFrame = frameIndex;
        mLastFrameTime = std::chrono::steady_clock::now();
        notifyFrameChange();
    }
}

void PlaybackController::nextFrame() {
    if (mTotalFrames == 0) {
        return;
    }
    
    uint32_t nextFrame = mCurrentFrame + 1;
    
    if (nextFrame >= mTotalFrames) {
        if (mLooping) {
            nextFrame = 0;
        } else {
            nextFrame = mTotalFrames - 1;
            // Auto-pause when reaching the end without looping
            if (mState == PlaybackState::PLAYING) {
                pause();
            }
        }
    }
    
    setFrame(nextFrame);
}

void PlaybackController::previousFrame() {
    if (mTotalFrames == 0) {
        return;
    }
    
    uint32_t prevFrame;
    
    if (mCurrentFrame == 0) {
        if (mLooping) {
            prevFrame = mTotalFrames - 1;
        } else {
            prevFrame = 0; // Stay at first frame
        }
    } else {
        prevFrame = mCurrentFrame - 1;
    }
    
    setFrame(prevFrame);
}

void PlaybackController::setFrameCount(uint32_t totalFrames) {
    mTotalFrames = totalFrames;
    
    // Ensure current frame is valid
    if (mCurrentFrame >= mTotalFrames && mTotalFrames > 0) {
        mCurrentFrame = mTotalFrames - 1;
    } else if (mTotalFrames == 0) {
        mCurrentFrame = 0;
        mState = PlaybackState::STOPPED;
    }
    
    // Always notify of frame count change
    notifyFrameChange();
}

// FPS and timing control

void PlaybackController::setFPS(float fps) {
    if (fps <= 0.0f) {
        throw thor::core::DataFormatError("FPS must be positive");
    }
    
    mFPS = fps;
    updateFrameDuration();
}

// Update method for timing-based playback

void PlaybackController::update() {
    if (mState != PlaybackState::PLAYING || mTotalFrames == 0) {
        return;
    }
    
    auto currentTime = std::chrono::steady_clock::now();
    auto timeSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - mLastFrameTime
    );
    
    if (timeSinceLastFrame >= mFrameDuration) {
        advanceFrame();
        mLastFrameTime = currentTime;
        mTotalFramesPlayed++;
    }
}

// Time-based queries

std::chrono::milliseconds PlaybackController::getFrameDuration() const {
    return mFrameDuration;
}

double PlaybackController::getElapsedSeconds() const {
    if (mState == PlaybackState::STOPPED || mTotalFramesPlayed == 0) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - mPlayStartTime
    );
    
    return elapsed.count() / 1000.0;
}

// Reset functionality

void PlaybackController::reset() {
    mState = PlaybackState::STOPPED;
    mCurrentFrame = 0;
    mTotalFramesPlayed = 0;
    mLastFrameTime = std::chrono::steady_clock::time_point();
    mPlayStartTime = std::chrono::steady_clock::time_point();
    notifyFrameChange();
}

// Private helper methods

void PlaybackController::updateFrameDuration() {
    // Convert FPS to milliseconds per frame
    double frameTimeMs = 1000.0 / static_cast<double>(mFPS);
    mFrameDuration = std::chrono::milliseconds(static_cast<int64_t>(frameTimeMs));
}

void PlaybackController::advanceFrame() {
    if (mTotalFrames == 0) {
        return;
    }
    
    uint32_t nextFrame = mCurrentFrame + 1;
    
    if (nextFrame >= mTotalFrames) {
        if (mLooping) {
            mCurrentFrame = 0;
        } else {
            // Reached end, pause playback
            mCurrentFrame = mTotalFrames - 1;
            pause();
            return;
        }
    } else {
        mCurrentFrame = nextFrame;
    }
    
    notifyFrameChange();
}

void PlaybackController::notifyFrameChange() {
    if (mFrameChangeCallback) {
        mFrameChangeCallback(mCurrentFrame, mTotalFrames);
    }
}

void PlaybackController::validateFrameIndex(uint32_t frameIndex) const {
    if (mTotalFrames == 0) {
        throw thor::core::DataFormatError("No frames available");
    }
    
    if (frameIndex >= mTotalFrames) {
        throw thor::core::DataFormatError(
            "Frame index " + std::to_string(frameIndex) + 
            " out of bounds (total frames: " + std::to_string(mTotalFrames) + ")"
        );
    }
}

} // namespace thor::playback 