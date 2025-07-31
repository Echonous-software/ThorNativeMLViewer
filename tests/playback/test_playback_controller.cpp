#include <gtest/gtest.h>
#include <playback/PlaybackController.hpp>
#include <core/Error.hpp>
#include <chrono>
#include <thread>

namespace thor::playback::test {

// Test fixture for PlaybackController tests
class PlaybackControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        controller = std::make_unique<PlaybackController>();
        callbackFrameIndex = 0;
        callbackTotalFrames = 0;
        callbackCalled = false;
        
        // Set up callback to track frame changes
        controller->setFrameChangeCallback([this](uint32_t current, uint32_t total) {
            callbackFrameIndex = current;
            callbackTotalFrames = total;
            callbackCalled = true;
        });
    }
    
    void TearDown() override {
        controller.reset();
    }
    
    void waitForFrameAdvancement(int milliseconds = 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        controller->update();
    }
    
    std::unique_ptr<PlaybackController> controller;
    uint32_t callbackFrameIndex;
    uint32_t callbackTotalFrames;
    bool callbackCalled;
};

// Basic construction and state tests

TEST_F(PlaybackControllerTest, DefaultConstruction) {
    // Assert
    EXPECT_EQ(controller->getState(), PlaybackState::STOPPED);
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    EXPECT_EQ(controller->getTotalFrames(), 0);
    EXPECT_FLOAT_EQ(controller->getFPS(), 30.0f);
    EXPECT_TRUE(controller->isLooping());
    EXPECT_TRUE(controller->isStopped());
    EXPECT_FALSE(controller->isPlaying());
    EXPECT_FALSE(controller->isPaused());
}

TEST_F(PlaybackControllerTest, SetFrameCount) {
    // Act
    controller->setFrameCount(10);
    
    // Assert
    EXPECT_EQ(controller->getTotalFrames(), 10);
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(callbackTotalFrames, 10);
}

// Playback control tests

TEST_F(PlaybackControllerTest, PlayWithoutFramesThrows) {
    // Act & Assert
    EXPECT_THROW(controller->play(), thor::core::DataFormatError);
}

TEST_F(PlaybackControllerTest, PlayPauseStopBasicFlow) {
    // Arrange
    controller->setFrameCount(5);
    callbackCalled = false; // Reset after setFrameCount callback
    
    // Act & Assert - Play
    controller->play();
    EXPECT_EQ(controller->getState(), PlaybackState::PLAYING);
    EXPECT_TRUE(controller->isPlaying());
    EXPECT_TRUE(callbackCalled);
    
    // Act & Assert - Pause
    callbackCalled = false;
    controller->pause();
    EXPECT_EQ(controller->getState(), PlaybackState::PAUSED);
    EXPECT_TRUE(controller->isPaused());
    EXPECT_TRUE(callbackCalled);
    
    // Act & Assert - Stop
    callbackCalled = false;
    controller->stop();
    EXPECT_EQ(controller->getState(), PlaybackState::STOPPED);
    EXPECT_TRUE(controller->isStopped());
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(PlaybackControllerTest, TogglePlayPause) {
    // Arrange
    controller->setFrameCount(5);
    
    // Act & Assert
    controller->togglePlayPause(); // Should start playing
    EXPECT_TRUE(controller->isPlaying());
    
    controller->togglePlayPause(); // Should pause
    EXPECT_TRUE(controller->isPaused());
    
    controller->togglePlayPause(); // Should resume playing
    EXPECT_TRUE(controller->isPlaying());
}

// Frame navigation tests

TEST_F(PlaybackControllerTest, SetFrameNavigation) {
    // Arrange
    controller->setFrameCount(5);
    
    // Act & Assert
    controller->setFrame(2);
    EXPECT_EQ(controller->getCurrentFrame(), 2);
    
    controller->setFrame(0);
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    
    controller->setFrame(4);
    EXPECT_EQ(controller->getCurrentFrame(), 4);
}

TEST_F(PlaybackControllerTest, SetFrameOutOfBoundsThrows) {
    // Arrange
    controller->setFrameCount(5);
    
    // Act & Assert
    EXPECT_THROW(controller->setFrame(5), thor::core::DataFormatError);
    EXPECT_THROW(controller->setFrame(100), thor::core::DataFormatError);
}

TEST_F(PlaybackControllerTest, NextFrameNavigation) {
    // Arrange
    controller->setFrameCount(3);
    
    // Act & Assert
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    
    controller->nextFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 1);
    
    controller->nextFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 2);
    
    // Next frame should loop back to 0 (looping enabled by default)
    controller->nextFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 0);
}

TEST_F(PlaybackControllerTest, NextFrameWithoutLooping) {
    // Arrange
    controller->setFrameCount(3);
    controller->setLooping(false);
    controller->setFrame(2); // Go to last frame
    controller->play();
    
    // Act
    controller->nextFrame();
    
    // Assert - Should stay at last frame and pause
    EXPECT_EQ(controller->getCurrentFrame(), 2);
    EXPECT_TRUE(controller->isPaused());
}

TEST_F(PlaybackControllerTest, PreviousFrameNavigation) {
    // Arrange
    controller->setFrameCount(3);
    controller->setFrame(2); // Start at frame 2
    
    // Act & Assert
    controller->previousFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 1);
    
    controller->previousFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    
    // Previous frame should loop to last frame (looping enabled)
    controller->previousFrame();
    EXPECT_EQ(controller->getCurrentFrame(), 2);
}

TEST_F(PlaybackControllerTest, PreviousFrameWithoutLooping) {
    // Arrange
    controller->setFrameCount(3);
    controller->setLooping(false);
    controller->setFrame(0); // Start at first frame
    
    // Act
    controller->previousFrame();
    
    // Assert - Should stay at first frame
    EXPECT_EQ(controller->getCurrentFrame(), 0);
}

// FPS and timing tests

TEST_F(PlaybackControllerTest, SetFPS) {
    // Act & Assert
    controller->setFPS(60.0f);
    EXPECT_FLOAT_EQ(controller->getFPS(), 60.0f);
    
    // Check frame duration calculation
    auto frameDuration = controller->getFrameDuration();
    EXPECT_EQ(frameDuration.count(), 16); // ~1000/60 = 16.67ms, rounded to 16ms
}

TEST_F(PlaybackControllerTest, SetInvalidFPSThrows) {
    // Act & Assert
    EXPECT_THROW(controller->setFPS(0.0f), thor::core::DataFormatError);
    EXPECT_THROW(controller->setFPS(-10.0f), thor::core::DataFormatError);
}

TEST_F(PlaybackControllerTest, TimingBasedPlayback) {
    // Arrange
    controller->setFrameCount(3);
    controller->setFPS(100.0f); // Fast FPS for quick testing (10ms per frame)
    controller->play();
    
    uint32_t initialFrame = controller->getCurrentFrame();
    
    // Act - Wait for frame advancement
    waitForFrameAdvancement(15); // Wait longer than frame duration
    
    // Assert - Frame should have advanced
    EXPECT_GT(controller->getCurrentFrame(), initialFrame);
    EXPECT_GT(controller->getTotalFramesPlayed(), 0);
}

// Looping functionality tests

TEST_F(PlaybackControllerTest, LoopingControlMethods) {
    // Assert default
    EXPECT_TRUE(controller->isLooping());
    
    // Act & Assert
    controller->setLooping(false);
    EXPECT_FALSE(controller->isLooping());
    
    controller->setLooping(true);
    EXPECT_TRUE(controller->isLooping());
}

TEST_F(PlaybackControllerTest, AutoPauseAtEndWithoutLooping) {
    // Arrange
    controller->setFrameCount(2);
    controller->setLooping(false);
    controller->setFPS(100.0f); // Fast FPS for testing
    controller->setFrame(1); // Start at last frame
    controller->play();
    
    // Act - Wait for automatic advancement
    waitForFrameAdvancement(15);
    
    // Assert - Should auto-pause at end
    EXPECT_EQ(controller->getCurrentFrame(), 1); // Still at last frame
    EXPECT_TRUE(controller->isPaused()); // Should be paused
}

TEST_F(PlaybackControllerTest, LoopingAtEndContinuesPlaying) {
    // Arrange
    controller->setFrameCount(2);
    controller->setLooping(true);
    controller->setFPS(100.0f); // Fast FPS for testing
    controller->setFrame(1); // Start at last frame
    controller->play();
    
    // Act - Wait for automatic advancement
    waitForFrameAdvancement(15);
    
    // Assert - Should loop back and continue playing
    EXPECT_EQ(controller->getCurrentFrame(), 0); // Should loop back to first frame
    EXPECT_TRUE(controller->isPlaying()); // Should still be playing
}

// Callback system tests

TEST_F(PlaybackControllerTest, FrameChangeCallback) {
    // Arrange
    controller->setFrameCount(5);
    callbackCalled = false;
    
    // Act
    controller->setFrame(3);
    
    // Assert
    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(callbackFrameIndex, 3);
    EXPECT_EQ(callbackTotalFrames, 5);
}

TEST_F(PlaybackControllerTest, ClearFrameChangeCallback) {
    // Arrange
    controller->setFrameCount(5);
    controller->clearFrameChangeCallback();
    callbackCalled = false;
    
    // Act
    controller->setFrame(2);
    
    // Assert - Callback should not be called
    EXPECT_FALSE(callbackCalled);
}

TEST_F(PlaybackControllerTest, CallbackDuringPlayback) {
    // Arrange
    controller->setFrameCount(3);
    controller->setFPS(100.0f); // Fast FPS
    controller->play();
    callbackCalled = false;
    
    // Act - Wait for frame advancement
    waitForFrameAdvancement(15);
    
    // Assert - Callback should be called when frame advances
    EXPECT_TRUE(callbackCalled);
}

// Statistics and timing query tests

TEST_F(PlaybackControllerTest, ElapsedTimeTracking) {
    // Arrange
    controller->setFrameCount(5);
    
    // Initially should be 0
    EXPECT_DOUBLE_EQ(controller->getElapsedSeconds(), 0.0);
    
    // Act
    controller->play();
    waitForFrameAdvancement(50); // Wait 50ms
    
    // Assert - Should have some elapsed time
    EXPECT_GT(controller->getElapsedSeconds(), 0.0);
}

TEST_F(PlaybackControllerTest, TotalFramesPlayedTracking) {
    // Arrange
    controller->setFrameCount(5);
    controller->setFPS(100.0f); // Fast FPS
    
    // Initially should be 0
    EXPECT_EQ(controller->getTotalFramesPlayed(), 0);
    
    // Act
    controller->play();
    waitForFrameAdvancement(25); // Wait for multiple frames
    
    // Assert - Should have played some frames
    EXPECT_GT(controller->getTotalFramesPlayed(), 0);
}

// Reset functionality tests

TEST_F(PlaybackControllerTest, ResetController) {
    // Arrange
    controller->setFrameCount(5);
    controller->setFrame(3);
    controller->play();
    waitForFrameAdvancement(20);
    
    // Act
    controller->reset();
    
    // Assert
    EXPECT_TRUE(controller->isStopped());
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    EXPECT_EQ(controller->getTotalFramesPlayed(), 0);
    EXPECT_DOUBLE_EQ(controller->getElapsedSeconds(), 0.0);
}

// Edge cases and error handling

TEST_F(PlaybackControllerTest, HandleZeroFrames) {
    // Arrange
    controller->setFrameCount(0);
    
    // Act & Assert - Methods should handle gracefully
    controller->nextFrame(); // Should not crash
    controller->previousFrame(); // Should not crash
    EXPECT_EQ(controller->getCurrentFrame(), 0);
    EXPECT_TRUE(controller->isStopped());
}

TEST_F(PlaybackControllerTest, SetFrameCountUpdatesCurrentFrame) {
    // Arrange
    controller->setFrameCount(10);
    controller->setFrame(5);
    
    // Act - Reduce frame count below current frame
    controller->setFrameCount(3);
    
    // Assert - Current frame should be adjusted
    EXPECT_EQ(controller->getCurrentFrame(), 2); // Last valid frame (3-1)
    EXPECT_EQ(controller->getTotalFrames(), 3);
}

TEST_F(PlaybackControllerTest, UpdateWithoutPlayingDoesNothing) {
    // Arrange
    controller->setFrameCount(5);
    // Don't start playing
    
    uint32_t initialFrame = controller->getCurrentFrame();
    
    // Act
    controller->update();
    
    // Assert - Frame should not change
    EXPECT_EQ(controller->getCurrentFrame(), initialFrame);
}

// Comprehensive timing accuracy test

TEST_F(PlaybackControllerTest, TimingAccuracyTest) {
    // Arrange
    controller->setFrameCount(10);
    controller->setFPS(50.0f); // 20ms per frame
    controller->play();
    
    auto startTime = std::chrono::steady_clock::now();
    uint32_t initialFrame = controller->getCurrentFrame();
    
    // Act - Run several update cycles
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(25)); // Slightly longer than frame time
        controller->update();
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Assert - Should have advanced approximately the right number of frames
    uint32_t expectedFrameAdvances = static_cast<uint32_t>(elapsedMs / 20); // 20ms per frame
    uint32_t actualFrameAdvances = controller->getCurrentFrame() - initialFrame;
    
    // Allow some tolerance for timing variations
    EXPECT_GE(actualFrameAdvances, expectedFrameAdvances - 1);
    EXPECT_LE(actualFrameAdvances, expectedFrameAdvances + 1);
}

} // namespace thor::playback::test 