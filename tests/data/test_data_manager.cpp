#include <gtest/gtest.h>
#include <data/DataManager.hpp>
#include <filesystem>
#include <fstream>
#include <vector>

class DataManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDataDir = std::filesystem::temp_directory_path() / "thor_test_data";
        std::filesystem::create_directories(testDataDir);
        
        dataManager = std::make_unique<thor::data::DataManager>();
    }
    
    void TearDown() override {
        // Clean up test files
        if (std::filesystem::exists(testDataDir)) {
            std::filesystem::remove_all(testDataDir);
        }
    }
    
    void createTestBinaryFile(const std::filesystem::path& filePath,
                             uint32_t width, uint32_t height, uint32_t channels,
                             uint32_t frameCount, thor::data::ImageDataType pixelType) {
        std::ofstream file(filePath, std::ios::binary);
        ASSERT_TRUE(file.is_open()) << "Failed to create test file: " << filePath;
        
        size_t pixelSize = (pixelType == thor::data::ImageDataType::UINT8) ? 1 : 4;
        size_t pixelsPerFrame = width * height * channels;
        
        for (uint32_t frame = 0; frame < frameCount; ++frame) {
            for (size_t pixel = 0; pixel < pixelsPerFrame; ++pixel) {
                if (pixelType == thor::data::ImageDataType::UINT8) {
                    uint8_t value = static_cast<uint8_t>((frame * 10 + pixel) % 256);
                    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
                } else {
                    float value = static_cast<float>(frame * 0.1f + pixel * 0.001f);
                    // Write as little-endian bytes
                    uint32_t intValue = *reinterpret_cast<uint32_t*>(&value);
                    uint8_t bytes[4] = {
                        static_cast<uint8_t>(intValue & 0xFF),
                        static_cast<uint8_t>((intValue >> 8) & 0xFF),
                        static_cast<uint8_t>((intValue >> 16) & 0xFF),
                        static_cast<uint8_t>((intValue >> 24) & 0xFF)
                    };
                    file.write(reinterpret_cast<const char*>(bytes), 4);
                }
            }
        }
        
        file.close();
    }
    
    std::filesystem::path testDataDir;
    std::unique_ptr<thor::data::DataManager> dataManager;
};

// Test basic initialization
TEST_F(DataManagerTest, Initialization) {
    EXPECT_FALSE(dataManager->hasSequence());
    EXPECT_EQ(dataManager->getFrameCount(), 0);
    EXPECT_EQ(dataManager->getWidth(), 0);
    EXPECT_EQ(dataManager->getHeight(), 0);
    EXPECT_EQ(dataManager->getChannels(), 0);
    EXPECT_EQ(dataManager->getPixelType(), thor::data::ImageDataType::UINT8);
    EXPECT_EQ(dataManager->getFPS(), 30.0f);
    
    // PlaybackController should be properly initialized
    EXPECT_TRUE(dataManager->getPlaybackController().isStopped());
    EXPECT_EQ(dataManager->getPlaybackController().getCurrentFrame(), 0);
    EXPECT_EQ(dataManager->getPlaybackController().getTotalFrames(), 0);
}

// Test loading UINT8 image sequence
TEST_F(DataManagerTest, LoadImageSequenceUint8) {
    auto testFile = testDataDir / "test_uint8.bin";
    uint32_t width = 64, height = 64, channels = 3, frameCount = 5;
    
    createTestBinaryFile(testFile, width, height, channels, frameCount, thor::data::ImageDataType::UINT8);
    
    EXPECT_TRUE(dataManager->loadImageSequence(testFile, width, height, thor::data::ImageDataType::UINT8, channels));
    
    EXPECT_TRUE(dataManager->hasSequence());
    EXPECT_EQ(dataManager->getFrameCount(), frameCount);
    EXPECT_EQ(dataManager->getWidth(), width);
    EXPECT_EQ(dataManager->getHeight(), height);
    EXPECT_EQ(dataManager->getChannels(), channels);
    EXPECT_EQ(dataManager->getPixelType(), thor::data::ImageDataType::UINT8);
    
    // PlaybackController should be configured
    EXPECT_EQ(dataManager->getPlaybackController().getTotalFrames(), frameCount);
}

// Test loading FLOAT32 image sequence
TEST_F(DataManagerTest, LoadImageSequenceFloat32) {
    auto testFile = testDataDir / "test_float32.bin";
    uint32_t width = 32, height = 32, channels = 1, frameCount = 3;
    
    createTestBinaryFile(testFile, width, height, channels, frameCount, thor::data::ImageDataType::FLOAT32);
    
    EXPECT_TRUE(dataManager->loadImageSequence(testFile, width, height, thor::data::ImageDataType::FLOAT32, channels));
    
    EXPECT_TRUE(dataManager->hasSequence());
    EXPECT_EQ(dataManager->getFrameCount(), frameCount);
    EXPECT_EQ(dataManager->getWidth(), width);
    EXPECT_EQ(dataManager->getHeight(), height);
    EXPECT_EQ(dataManager->getChannels(), channels);
    EXPECT_EQ(dataManager->getPixelType(), thor::data::ImageDataType::FLOAT32);
}

// Test convenience methods for 128x128 and 224x224
TEST_F(DataManagerTest, ConvenienceMethods) {
    // Test 128x128
    auto testFile128 = testDataDir / "test_128.bin";
    createTestBinaryFile(testFile128, 128, 128, 3, 2, thor::data::ImageDataType::UINT8);
    
    EXPECT_TRUE(dataManager->loadImageSequence128(testFile128, thor::data::ImageDataType::UINT8));
    EXPECT_EQ(dataManager->getWidth(), 128);
    EXPECT_EQ(dataManager->getHeight(), 128);
    
    // Test 224x224
    auto testFile224 = testDataDir / "test_224.bin";
    createTestBinaryFile(testFile224, 224, 224, 3, 2, thor::data::ImageDataType::FLOAT32);
    
    EXPECT_TRUE(dataManager->loadImageSequence224(testFile224, thor::data::ImageDataType::FLOAT32));
    EXPECT_EQ(dataManager->getWidth(), 224);
    EXPECT_EQ(dataManager->getHeight(), 224);
}

// Test getCurrentImageView integration with PlaybackController
TEST_F(DataManagerTest, GetCurrentImageView) {
    auto testFile = testDataDir / "test_current_view.bin";
    uint32_t width = 16, height = 16, channels = 3, frameCount = 4;
    
    createTestBinaryFile(testFile, width, height, channels, frameCount, thor::data::ImageDataType::UINT8);
    dataManager->loadImageSequence(testFile, width, height, thor::data::ImageDataType::UINT8, channels);
    
    // Initially should return first frame
    auto currentView = dataManager->getCurrentImageView();
    ASSERT_TRUE(currentView.has_value());
    EXPECT_EQ(currentView->getWidth(), width);
    EXPECT_EQ(currentView->getHeight(), height);
    EXPECT_EQ(currentView->getChannels(), channels);
    
    // Change frame through playback controller
    dataManager->getPlaybackController().setFrame(2);
    currentView = dataManager->getCurrentImageView();
    ASSERT_TRUE(currentView.has_value());
    
    // Should still have same dimensions but different frame
    EXPECT_EQ(currentView->getWidth(), width);
    EXPECT_EQ(currentView->getHeight(), height);
    EXPECT_EQ(currentView->getChannels(), channels);
}

// Test getImageView with specific frame index
TEST_F(DataManagerTest, GetImageViewByIndex) {
    auto testFile = testDataDir / "test_index_view.bin";
    uint32_t width = 8, height = 8, channels = 1, frameCount = 3;
    
    createTestBinaryFile(testFile, width, height, channels, frameCount, thor::data::ImageDataType::UINT8);
    dataManager->loadImageSequence(testFile, width, height, thor::data::ImageDataType::UINT8, channels);
    
    // Test valid frame indices
    for (uint32_t i = 0; i < frameCount; ++i) {
        auto view = dataManager->getImageView(i);
        ASSERT_TRUE(view.has_value()) << "Failed to get view for frame " << i;
        EXPECT_EQ(view->getWidth(), width);
        EXPECT_EQ(view->getHeight(), height);
        EXPECT_EQ(view->getChannels(), channels);
    }
    
    // Test invalid frame index
    auto invalidView = dataManager->getImageView(frameCount);
    EXPECT_FALSE(invalidView.has_value());
}

// Test playback controller integration
TEST_F(DataManagerTest, PlaybackControllerIntegration) {
    auto testFile = testDataDir / "test_playback.bin";
    uint32_t width = 10, height = 10, channels = 3, frameCount = 5;
    
    createTestBinaryFile(testFile, width, height, channels, frameCount, thor::data::ImageDataType::UINT8);
    dataManager->loadImageSequence(testFile, width, height, thor::data::ImageDataType::UINT8, channels);
    
    auto& playbackController = dataManager->getPlaybackController();
    
    // Test frame navigation
    EXPECT_EQ(playbackController.getCurrentFrame(), 0);
    
    playbackController.nextFrame();
    EXPECT_EQ(playbackController.getCurrentFrame(), 1);
    
    playbackController.setFrame(3);
    EXPECT_EQ(playbackController.getCurrentFrame(), 3);
    
    playbackController.previousFrame();
    EXPECT_EQ(playbackController.getCurrentFrame(), 2);
    
    // Test that getCurrentImageView tracks playback controller
    auto currentView = dataManager->getCurrentImageView();
    ASSERT_TRUE(currentView.has_value());
    
    // Verify the image view corresponds to the current frame
    auto specificView = dataManager->getImageView(2);
    ASSERT_TRUE(specificView.has_value());
    
    EXPECT_EQ(currentView->getWidth(), specificView->getWidth());
    EXPECT_EQ(currentView->getHeight(), specificView->getHeight());
    EXPECT_EQ(currentView->getChannels(), specificView->getChannels());
}

// Test error handling
TEST_F(DataManagerTest, ErrorHandling) {
    // Test loading non-existent file
    auto nonExistentFile = testDataDir / "does_not_exist.bin";
    EXPECT_FALSE(dataManager->loadImageSequence(nonExistentFile, 64, 64, thor::data::ImageDataType::UINT8));
    EXPECT_FALSE(dataManager->hasSequence());
    
    // Test getting views when no sequence is loaded
    EXPECT_FALSE(dataManager->getCurrentImageView().has_value());
    EXPECT_FALSE(dataManager->getImageView(0).has_value());
}

// Test clear functionality
TEST_F(DataManagerTest, Clear) {
    auto testFile = testDataDir / "test_clear.bin";
    createTestBinaryFile(testFile, 32, 32, 3, 2, thor::data::ImageDataType::UINT8);
    
    // Load sequence
    EXPECT_TRUE(dataManager->loadImageSequence(testFile, 32, 32, thor::data::ImageDataType::UINT8));
    EXPECT_TRUE(dataManager->hasSequence());
    
    // Clear
    dataManager->clear();
    EXPECT_FALSE(dataManager->hasSequence());
    EXPECT_EQ(dataManager->getFrameCount(), 0);
    EXPECT_FALSE(dataManager->getCurrentImageView().has_value());
    
    // PlaybackController should be reset
    EXPECT_TRUE(dataManager->getPlaybackController().isStopped());
    EXPECT_EQ(dataManager->getPlaybackController().getCurrentFrame(), 0);
}

// Test multiple loads
TEST_F(DataManagerTest, MultipleLoads) {
    auto testFile1 = testDataDir / "test1.bin";
    auto testFile2 = testDataDir / "test2.bin";
    
    createTestBinaryFile(testFile1, 16, 16, 3, 3, thor::data::ImageDataType::UINT8);
    createTestBinaryFile(testFile2, 32, 32, 1, 5, thor::data::ImageDataType::FLOAT32);
    
    // Load first sequence
    EXPECT_TRUE(dataManager->loadImageSequence(testFile1, 16, 16, thor::data::ImageDataType::UINT8));
    EXPECT_EQ(dataManager->getWidth(), 16);
    EXPECT_EQ(dataManager->getFrameCount(), 3);
    
    // Load second sequence (should replace first)
    EXPECT_TRUE(dataManager->loadImageSequence(testFile2, 32, 32, thor::data::ImageDataType::FLOAT32, 1));
    EXPECT_EQ(dataManager->getWidth(), 32);
    EXPECT_EQ(dataManager->getFrameCount(), 5);
    EXPECT_EQ(dataManager->getChannels(), 1);
    EXPECT_EQ(dataManager->getPixelType(), thor::data::ImageDataType::FLOAT32);
} 