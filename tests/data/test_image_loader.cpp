#include <gtest/gtest.h>
#include <data/ImageLoader.hpp>
#include <core/Error.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <memory>

namespace thor::data::test {

using thor::data::ImageLoader;

// Test fixture for ImageLoader tests
class ImageLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for test files
        testDir = std::filesystem::temp_directory_path() / "thor_imageloader_test";
        std::filesystem::create_directories(testDir);
        
        // Create test binary files
        createTestUint8File();
        createTestFloat32File();
        createInvalidFiles();
    }
    
    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(testDir);
    }
    
    void createTestUint8File() {
        // Create a 2x2x3 uint8 test file with 2 frames (24 bytes total)
        uint8TestFile = testDir / "test_uint8.bin";
        std::vector<uint8_t> data = {
            // Frame 1: 2x2x3 = 12 bytes
            1, 2, 3,     4, 5, 6,        // Row 1: pixel(0,0), pixel(1,0)
            7, 8, 9,     10, 11, 12,     // Row 2: pixel(0,1), pixel(1,1)
            // Frame 2: 2x2x3 = 12 bytes  
            13, 14, 15,  16, 17, 18,     // Row 1: pixel(0,0), pixel(1,0)
            19, 20, 21,  22, 23, 24      // Row 2: pixel(0,1), pixel(1,1)
        };
        
        std::ofstream file(uint8TestFile, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    
    void createTestFloat32File() {
        // Create a 2x2x3 float32 test file with 2 frames
        float32TestFile = testDir / "test_float32.bin";
        std::vector<float> floatData = {
            // Frame 1: 2x2x3 = 12 floats
            0.1f, 0.2f, 0.3f,   0.4f, 0.5f, 0.6f,
            0.7f, 0.8f, 0.9f,   1.0f, 1.1f, 1.2f,
            // Frame 2: 2x2x3 = 12 floats
            1.3f, 1.4f, 1.5f,   1.6f, 1.7f, 1.8f,
            1.9f, 2.0f, 2.1f,   2.2f, 2.3f, 2.4f
        };
        
        std::ofstream file(float32TestFile, std::ios::binary);
        
        // Write as little-endian bytes
        for (float value : floatData) {
            uint32_t rawValue;
            std::memcpy(&rawValue, &value, sizeof(float));
            
            // Write in little-endian order
            uint8_t bytes[4] = {
                static_cast<uint8_t>(rawValue & 0xFF),
                static_cast<uint8_t>((rawValue >> 8) & 0xFF),
                static_cast<uint8_t>((rawValue >> 16) & 0xFF),
                static_cast<uint8_t>((rawValue >> 24) & 0xFF)
            };
            
            file.write(reinterpret_cast<const char*>(bytes), 4);
        }
    }
    
    void createInvalidFiles() {
        // Empty file
        emptyFile = testDir / "empty.bin";
        std::ofstream(emptyFile).close();
        
        // File with invalid size (not multiple of frame size)
        invalidSizeFile = testDir / "invalid_size.bin";
        std::vector<uint8_t> invalidData = {1, 2, 3, 4, 5}; // 5 bytes, not a multiple of 12
        std::ofstream file(invalidSizeFile, std::ios::binary);
        file.write(reinterpret_cast<const char*>(invalidData.data()), invalidData.size());
    }
    
    std::filesystem::path testDir;
    std::filesystem::path uint8TestFile;
    std::filesystem::path float32TestFile;
    std::filesystem::path emptyFile;
    std::filesystem::path invalidSizeFile;
    ImageLoader loader;
};

// Basic loading tests

TEST_F(ImageLoaderTest, LoadUint8ImageSequence) {
    // Arrange & Act
    auto sequence = loader.loadImageSequence(uint8TestFile, 2, 2, ImageDataType::UINT8);
    
    // Assert
    ASSERT_NE(sequence, nullptr);
    EXPECT_EQ(sequence->getWidth(), 2);
    EXPECT_EQ(sequence->getHeight(), 2);
    EXPECT_EQ(sequence->getChannels(), 3);
    EXPECT_EQ(sequence->getPixelType(), ImageDataType::UINT8);
    EXPECT_EQ(sequence->getFrameCount(), 2);
    EXPECT_FLOAT_EQ(sequence->getFPS(), 30.0f); // Default FPS
    
    // Test frame data
    auto view0 = sequence->getImageView(0);
    auto data0 = view0.getDataAsUint8();
    EXPECT_EQ(data0[0], 1);   // First pixel, first channel
    EXPECT_EQ(data0[1], 2);   // First pixel, second channel
    EXPECT_EQ(data0[2], 3);   // First pixel, third channel
    
    auto view1 = sequence->getImageView(1);
    auto data1 = view1.getDataAsUint8();
    EXPECT_EQ(data1[0], 13);  // First pixel of second frame
}

TEST_F(ImageLoaderTest, LoadFloat32ImageSequence) {
    // Arrange & Act
    auto sequence = loader.loadImageSequence(float32TestFile, 2, 2, ImageDataType::FLOAT32);
    
    // Assert
    ASSERT_NE(sequence, nullptr);
    EXPECT_EQ(sequence->getPixelType(), ImageDataType::FLOAT32);
    EXPECT_EQ(sequence->getFrameCount(), 2);
    
    // Test frame data with little-endian conversion
    auto view0 = sequence->getImageView(0);
    auto data0 = view0.getDataAsFloat32();
    EXPECT_FLOAT_EQ(data0[0], 0.1f);   // First pixel, first channel
    EXPECT_FLOAT_EQ(data0[1], 0.2f);   // First pixel, second channel
    EXPECT_FLOAT_EQ(data0[2], 0.3f);   // First pixel, third channel
    
    auto view1 = sequence->getImageView(1);
    auto data1 = view1.getDataAsFloat32();
    EXPECT_FLOAT_EQ(data1[0], 1.3f);   // First pixel of second frame
}

TEST_F(ImageLoaderTest, LoadImageSequence128Convenience) {
    // Arrange - Create a 128x128x3 uint8 test file with 1 frame
    auto file128 = testDir / "test_128.bin";
    size_t fileSize = 128 * 128 * 3; // 1 frame
    std::vector<uint8_t> data(fileSize, 42); // Fill with test value
    std::ofstream(file128, std::ios::binary).write(reinterpret_cast<const char*>(data.data()), fileSize);
    
    // Act
    auto sequence = loader.loadImageSequence128(file128, ImageDataType::UINT8);
    
    // Assert
    ASSERT_NE(sequence, nullptr);
    EXPECT_EQ(sequence->getWidth(), 128);
    EXPECT_EQ(sequence->getHeight(), 128);
    EXPECT_EQ(sequence->getChannels(), 3);
    EXPECT_EQ(sequence->getFrameCount(), 1);
    
    auto view = sequence->getImageView(0);
    auto frameData = view.getDataAsUint8();
    EXPECT_EQ(frameData[0], 42);
}

TEST_F(ImageLoaderTest, LoadImageSequence224Convenience) {
    // Arrange - Create a 224x224x3 uint8 test file with 1 frame
    auto file224 = testDir / "test_224.bin";
    size_t fileSize = 224 * 224 * 3; // 1 frame
    std::vector<uint8_t> data(fileSize, 99); // Fill with test value
    std::ofstream(file224, std::ios::binary).write(reinterpret_cast<const char*>(data.data()), fileSize);
    
    // Act
    auto sequence = loader.loadImageSequence224(file224, ImageDataType::UINT8);
    
    // Assert
    ASSERT_NE(sequence, nullptr);
    EXPECT_EQ(sequence->getWidth(), 224);
    EXPECT_EQ(sequence->getHeight(), 224);
    EXPECT_EQ(sequence->getChannels(), 3);
    EXPECT_EQ(sequence->getFrameCount(), 1);
    
    auto view = sequence->getImageView(0);
    auto frameData = view.getDataAsUint8();
    EXPECT_EQ(frameData[0], 99);
}

// Frame count calculation tests

TEST_F(ImageLoaderTest, CalculateFrameCount) {
    // Act & Assert
    uint32_t count = ImageLoader::calculateFrameCount(uint8TestFile, 2, 2, 3,ImageDataType::UINT8);
    EXPECT_EQ(count, 2);
    
    count = ImageLoader::calculateFrameCount(float32TestFile, 2, 2, 3,ImageDataType::FLOAT32);
    EXPECT_EQ(count, 2);
}

// Error handling tests

TEST_F(ImageLoaderTest, ThrowOnNonExistentFile) {
    // Arrange
    auto nonExistentFile = testDir / "does_not_exist.bin";
    
    // Act & Assert
    EXPECT_THROW(loader.loadImageSequence(nonExistentFile, 2, 2, ImageDataType::UINT8), 
                 thor::core::DataFormatError);
}

TEST_F(ImageLoaderTest, ThrowOnEmptyFile) {
    // Act & Assert
    EXPECT_THROW(loader.loadImageSequence(emptyFile, 2, 2, ImageDataType::UINT8),
                 thor::core::DataFormatError);
}

TEST_F(ImageLoaderTest, ThrowOnInvalidFileSize) {
    // Act & Assert
    EXPECT_THROW(loader.loadImageSequence(invalidSizeFile, 2, 2, ImageDataType::UINT8),
                 thor::core::DataFormatError);
}

TEST_F(ImageLoaderTest, ThrowOnInvalidDimensions) {
    // Act & Assert
    EXPECT_THROW(loader.loadImageSequence(uint8TestFile, 0, 2,ImageDataType::UINT8),
                 thor::core::DataFormatError);
    EXPECT_THROW(loader.loadImageSequence(uint8TestFile, 2, 0, ImageDataType::UINT8),
                 thor::core::DataFormatError);
    EXPECT_THROW(loader.loadImageSequence(uint8TestFile, 2, 2, ImageDataType::UINT8, 0),
                 thor::core::DataFormatError);
    EXPECT_THROW(loader.loadImageSequence(uint8TestFile, 2, 2, ImageDataType::UINT8, 5),
                 thor::core::DataFormatError);
}

TEST_F(ImageLoaderTest, ThrowOnEmptyFilePath) {
    // Act & Assert
    EXPECT_THROW(loader.loadImageSequence("", 2, 2, ImageDataType::UINT8),
                 thor::core::DataFormatError);
}

// Byte ordering tests

TEST_F(ImageLoaderTest, LittleEndianFloat32Conversion) {
    // Arrange - Create file with known float32 values in little-endian format
    auto testFile = testDir / "endian_test.bin";
    std::ofstream file(testFile, std::ios::binary);
    
    // Write known float value (3.14159) in little-endian format
    float testValue = 3.14159f;
    uint32_t rawValue;
    std::memcpy(&rawValue, &testValue, sizeof(float));
    
    uint8_t bytes[4] = {
        static_cast<uint8_t>(rawValue & 0xFF),
        static_cast<uint8_t>((rawValue >> 8) & 0xFF),
        static_cast<uint8_t>((rawValue >> 16) & 0xFF),
        static_cast<uint8_t>((rawValue >> 24) & 0xFF)
    };
    
    file.write(reinterpret_cast<const char*>(bytes), 4);
    file.close();
    
    // Act
    auto sequence = loader.loadImageSequence(testFile, 1, 1,ImageDataType::FLOAT32, 1);
    
    // Assert
    ASSERT_NE(sequence, nullptr);
    auto view = sequence->getImageView(0);
    auto data = view.getDataAsFloat32();
    EXPECT_FLOAT_EQ(data[0], testValue);
}

// Custom FPS and channels tests

TEST_F(ImageLoaderTest, CustomFPSAndChannels) {
    // Arrange - Create 2x2x1 grayscale file
    auto grayscaleFile = testDir / "grayscale.bin";
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8}; // 2 frames of 2x2x1
    std::ofstream(grayscaleFile, std::ios::binary).write(reinterpret_cast<const char*>(data.data()), data.size());
    
    // Act
    auto sequence = loader.loadImageSequence(grayscaleFile, 2, 2,ImageDataType::UINT8, 1, 60.0f);
    
    // Assert
    EXPECT_EQ(sequence->getChannels(), 1);
    EXPECT_FLOAT_EQ(sequence->getFPS(), 60.0f);
    EXPECT_EQ(sequence->getFrameCount(), 2);
}

// Multi-frame tests

TEST_F(ImageLoaderTest, MultipleFrameAccess) {
    // Act
    auto sequence = loader.loadImageSequence(uint8TestFile, 2, 2, ImageDataType::UINT8);
    
    // Assert - Test each frame has different data
    auto view0 = sequence->getImageView(0);
    auto view1 = sequence->getImageView(1);
    
    auto data0 = view0.getDataAsUint8();
    auto data1 = view1.getDataAsUint8();
    
    // First frame starts with 1, second frame starts with 13
    EXPECT_EQ(data0[0], 1);
    EXPECT_EQ(data1[0], 13);
    
    // Verify frame data is correctly separated
    EXPECT_EQ(data0[11], 12);  // Last pixel of first frame
    EXPECT_EQ(data1[11], 24);  // Last pixel of second frame
}

// Large file handling tests

TEST_F(ImageLoaderTest, HandleLargerSequence) {
    // Arrange - Create a larger test file (10 frames of 4x4x3)
    auto largeFile = testDir / "large_sequence.bin";
    size_t frameSize = 4 * 4 * 3; // 48 bytes per frame
    size_t numFrames = 10;
    std::vector<uint8_t> data(frameSize * numFrames);
    
    // Fill with frame-specific patterns
    for (size_t frame = 0; frame < numFrames; ++frame) {
        for (size_t i = 0; i < frameSize; ++i) {
            data[frame * frameSize + i] = static_cast<uint8_t>(frame * 10 + (i % 10));
        }
    }
    
    std::ofstream(largeFile, std::ios::binary).write(reinterpret_cast<const char*>(data.data()), data.size());
    
    // Act
    auto sequence = loader.loadImageSequence(largeFile, 4, 4, ImageDataType::UINT8);
    
    // Assert
    EXPECT_EQ(sequence->getFrameCount(), 10);
    
    // Test first and last frames
    auto firstFrame = sequence->getImageView(0);
    auto lastFrame = sequence->getImageView(9);
    
    auto firstData = firstFrame.getDataAsUint8();
    auto lastData = lastFrame.getDataAsUint8();
    
    EXPECT_EQ(firstData[0], 0);  // Frame 0, first pixel
    EXPECT_EQ(lastData[0], 90);  // Frame 9, first pixel (9*10+0)
}

} // namespace thor::data::test 