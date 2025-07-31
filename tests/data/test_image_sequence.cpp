#include <gtest/gtest.h>
#include <data/ImageSequence.hpp>
#include <core/Error.hpp>
#include <vector>

namespace thor::data::test {

// Test fixture for ImageView tests
class ImageViewTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test data
        uint8Data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        floatData = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f};
    }
    
    std::vector<uint8_t> uint8Data;
    std::vector<float> floatData;
};

// Test fixture for ImageSequence tests
class ImageSequenceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test frame data for 2x2 RGB images
        uint8Frame1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}; // 2x2x3 = 12 bytes
        uint8Frame2 = {13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
        
        floatFrame1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f};
        floatFrame2 = {1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f, 2.1f, 2.2f, 2.3f, 2.4f};
    }
    
    std::vector<uint8_t> uint8Frame1, uint8Frame2;
    std::vector<float> floatFrame1, floatFrame2;
};

// ImageView Tests

TEST_F(ImageViewTest, ConstructUint8ImageView) {
    // Arrange & Act
    ImageView view(std::span<const uint8_t>(uint8Data), 2, 2, 3);
    
    // Assert
    EXPECT_EQ(view.getWidth(), 2);
    EXPECT_EQ(view.getHeight(), 2);
    EXPECT_EQ(view.getChannels(), 3);
    EXPECT_EQ(view.getPixelType(), ImageDataType::UINT8);
    EXPECT_EQ(view.getDataSizeBytes(), 12);
}

TEST_F(ImageViewTest, ConstructFloat32ImageView) {
    // Arrange & Act
    ImageView view(std::span<const float>(floatData), 2, 2, 3);
    
    // Assert
    EXPECT_EQ(view.getWidth(), 2);
    EXPECT_EQ(view.getHeight(), 2);
    EXPECT_EQ(view.getChannels(), 3);
    EXPECT_EQ(view.getPixelType(), ImageDataType::FLOAT32);
    EXPECT_EQ(view.getDataSizeBytes(), 48); // 12 floats * 4 bytes
}

TEST_F(ImageViewTest, GetDataAsCorrectType) {
    // Arrange
    ImageView uint8View(std::span<const uint8_t>(uint8Data), 2, 2, 3);
    ImageView floatView(std::span<const float>(floatData), 2, 2, 3);
    
    // Act & Assert
    auto uint8Span = uint8View.getDataAsUint8();
    EXPECT_EQ(uint8Span.size(), 12);
    EXPECT_EQ(uint8Span[0], 1);
    
    auto floatSpan = floatView.getDataAsFloat32();
    EXPECT_EQ(floatSpan.size(), 12);
    EXPECT_FLOAT_EQ(floatSpan[0], 0.1f);
}

TEST_F(ImageViewTest, ThrowOnWrongDataTypeAccess) {
    // Arrange
    ImageView uint8View(std::span<const uint8_t>(uint8Data), 2, 2, 3);
    ImageView floatView(std::span<const float>(floatData), 2, 2, 3);
    
    // Act & Assert
    EXPECT_THROW(uint8View.getDataAsFloat32(), core::DataFormatError);
    EXPECT_THROW(floatView.getDataAsUint8(), core::DataFormatError);
}

TEST_F(ImageViewTest, PixelAccess) {
    // Arrange
    ImageView view(std::span<const uint8_t>(uint8Data), 2, 2, 3);
    
    // Act
    const void* pixel00 = view.getPixel(0, 0);
    const void* pixel11 = view.getPixel(1, 1);
    
    // Assert
    EXPECT_EQ(*static_cast<const uint8_t*>(pixel00), 1);
    EXPECT_EQ(*static_cast<const uint8_t*>(pixel11), 10); // pixel (1,1) starts at index 9
}

TEST_F(ImageViewTest, ThrowOnInvalidDimensions) {
    // Act & Assert
    EXPECT_THROW(ImageView(std::span<const uint8_t>(uint8Data), 0, 2, 3), core::DataFormatError);
    EXPECT_THROW(ImageView(std::span<const uint8_t>(uint8Data), 2, 0, 3), core::DataFormatError);
    EXPECT_THROW(ImageView(std::span<const uint8_t>(uint8Data), 2, 2, 0), core::DataFormatError);
    EXPECT_THROW(ImageView(std::span<const uint8_t>(uint8Data), 2, 2, 5), core::DataFormatError); // > 4 channels
}

TEST_F(ImageViewTest, ThrowOnInsufficientData) {
    // Arrange
    std::vector<uint8_t> smallData = {1, 2, 3}; // Too small for 2x2x3
    
    // Act & Assert
    EXPECT_THROW(ImageView(std::span<const uint8_t>(smallData), 2, 2, 3), core::DataFormatError);
}

TEST_F(ImageViewTest, ThrowOnOutOfBoundsPixelAccess) {
    // Arrange
    ImageView view(std::span<const uint8_t>(uint8Data), 2, 2, 3);
    
    // Act & Assert
    EXPECT_THROW(view.getPixel(2, 0), core::DataFormatError); // x out of bounds
    EXPECT_THROW(view.getPixel(0, 2), core::DataFormatError); // y out of bounds
    EXPECT_THROW(view.getPixel(2, 2), core::DataFormatError); // both out of bounds
}

// ImageSequence Tests

TEST_F(ImageSequenceTest, ConstructUint8ImageSequence) {
    // Arrange & Act
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8, 30.0f);
    
    // Assert
    EXPECT_EQ(sequence.getWidth(), 2);
    EXPECT_EQ(sequence.getHeight(), 2);
    EXPECT_EQ(sequence.getChannels(), 3);
    EXPECT_EQ(sequence.getPixelType(), ImageDataType::UINT8);
    EXPECT_FLOAT_EQ(sequence.getFPS(), 30.0f);
    EXPECT_EQ(sequence.getFrameCount(), 0);
    EXPECT_EQ(sequence.getFrameSizeBytes(), 12);
}

TEST_F(ImageSequenceTest, ConstructFloat32ImageSequence) {
    // Arrange & Act
    ImageSequence sequence(2, 2, 3, ImageDataType::FLOAT32, 60.0f);
    
    // Assert
    EXPECT_EQ(sequence.getPixelType(), ImageDataType::FLOAT32);
    EXPECT_FLOAT_EQ(sequence.getFPS(), 60.0f);
    EXPECT_EQ(sequence.getFrameSizeBytes(), 48); // 12 floats * 4 bytes
}

TEST_F(ImageSequenceTest, AddUint8Frames) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    
    // Act
    sequence.addFrame(std::span<const uint8_t>(uint8Frame1));
    sequence.addFrame(std::span<const uint8_t>(uint8Frame2));
    
    // Assert
    EXPECT_EQ(sequence.getFrameCount(), 2);
    EXPECT_EQ(sequence.getTotalSizeBytes(), 24); // 2 frames * 12 bytes
}

TEST_F(ImageSequenceTest, AddFloat32Frames) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::FLOAT32);
    
    // Act
    sequence.addFrame(std::span<const float>(floatFrame1));
    sequence.addFrame(std::span<const float>(floatFrame2));
    
    // Assert
    EXPECT_EQ(sequence.getFrameCount(), 2);
    EXPECT_EQ(sequence.getTotalSizeBytes(), 96); // 2 frames * 48 bytes
}

TEST_F(ImageSequenceTest, GetImageViewZeroCopy) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    sequence.addFrame(std::span<const uint8_t>(uint8Frame1));
    sequence.addFrame(std::span<const uint8_t>(uint8Frame2));
    
    // Act
    ImageView view0 = sequence.getImageView(0);
    ImageView view1 = sequence.getImageView(1);
    
    // Assert
    EXPECT_EQ(view0.getWidth(), 2);
    EXPECT_EQ(view0.getHeight(), 2);
    EXPECT_EQ(view0.getChannels(), 3);
    EXPECT_EQ(view0.getPixelType(), ImageDataType::UINT8);
    
    auto data0 = view0.getDataAsUint8();
    auto data1 = view1.getDataAsUint8();
    
    EXPECT_EQ(data0[0], 1);   // First pixel of first frame
    EXPECT_EQ(data1[0], 13);  // First pixel of second frame
}

TEST_F(ImageSequenceTest, ReserveFrames) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    
    // Act
    sequence.reserveFrames(100);
    
    // Assert - Should not throw or crash, memory is pre-allocated
    EXPECT_EQ(sequence.getFrameCount(), 0); // No frames added yet
}

TEST_F(ImageSequenceTest, ClearSequence) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    sequence.addFrame(std::span<const uint8_t>(uint8Frame1));
    sequence.addFrame(std::span<const uint8_t>(uint8Frame2));
    
    // Act
    sequence.clear();
    
    // Assert
    EXPECT_EQ(sequence.getFrameCount(), 0);
    EXPECT_EQ(sequence.getTotalSizeBytes(), 0);
}

TEST_F(ImageSequenceTest, ThrowOnInvalidConstruction) {
    // Act & Assert
    EXPECT_THROW(ImageSequence(0, 2, 3, ImageDataType::UINT8), core::DataFormatError);
    EXPECT_THROW(ImageSequence(2, 0, 3, ImageDataType::UINT8), core::DataFormatError);
    EXPECT_THROW(ImageSequence(2, 2, 0, ImageDataType::UINT8), core::DataFormatError);
    EXPECT_THROW(ImageSequence(2, 2, 5, ImageDataType::UINT8), core::DataFormatError); // > 4 channels
    EXPECT_THROW(ImageSequence(2, 2, 3, ImageDataType::UINT8, 0.0f), core::DataFormatError); // Invalid FPS
    EXPECT_THROW(ImageSequence(2, 2, 3, ImageDataType::UINT8, -1.0f), core::DataFormatError); // Negative FPS
}

TEST_F(ImageSequenceTest, ThrowOnWrongFrameType) {
    // Arrange
    ImageSequence uint8Sequence(2, 2, 3, ImageDataType::UINT8);
    ImageSequence floatSequence(2, 2, 3, ImageDataType::FLOAT32);
    
    // Act & Assert
    EXPECT_THROW(uint8Sequence.addFrame(std::span<const float>(floatFrame1)), core::DataFormatError);
    EXPECT_THROW(floatSequence.addFrame(std::span<const uint8_t>(uint8Frame1)), core::DataFormatError);
}

TEST_F(ImageSequenceTest, ThrowOnWrongFrameSize) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    std::vector<uint8_t> wrongSizeFrame = {1, 2, 3, 4, 5}; // Too small
    
    // Act & Assert
    EXPECT_THROW(sequence.addFrame(std::span<const uint8_t>(wrongSizeFrame)), core::DataFormatError);
}

TEST_F(ImageSequenceTest, ThrowOnInvalidFrameIndex) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    sequence.addFrame(std::span<const uint8_t>(uint8Frame1));
    
    // Act & Assert
    EXPECT_THROW(sequence.getImageView(1), core::DataFormatError); // Index 1 when only 1 frame (index 0)
    EXPECT_THROW(sequence.getImageView(100), core::DataFormatError); // Way out of bounds
}

TEST_F(ImageSequenceTest, AddFrameWithRawPointer) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    
    // Act
    sequence.addFrame(uint8Frame1.data(), uint8Frame1.size());
    
    // Assert
    EXPECT_EQ(sequence.getFrameCount(), 1);
    ImageView view = sequence.getImageView(0);
    auto data = view.getDataAsUint8();
    EXPECT_EQ(data[0], 1);
}

TEST_F(ImageSequenceTest, ThrowOnNullFrameData) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    
    // Act & Assert
    EXPECT_THROW(sequence.addFrame(nullptr, 12), core::DataFormatError);
}

TEST_F(ImageSequenceTest, SetFPS) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8, 30.0f);
    
    // Act
    sequence.setFPS(60.0f);
    
    // Assert
    EXPECT_FLOAT_EQ(sequence.getFPS(), 60.0f);
}

// Edge case and integration tests

TEST_F(ImageSequenceTest, LargeSequenceHandling) {
    // Arrange
    ImageSequence sequence(2, 2, 3, ImageDataType::UINT8);
    sequence.reserveFrames(1000); // Pre-allocate for performance
    
    // Act - Add many frames
    for (int i = 0; i < 100; ++i) {
        sequence.addFrame(std::span<const uint8_t>(uint8Frame1));
    }
    
    // Assert
    EXPECT_EQ(sequence.getFrameCount(), 100);
    EXPECT_EQ(sequence.getTotalSizeBytes(), 1200); // 100 * 12 bytes
    
    // Test access to first and last frames
    ImageView firstFrame = sequence.getImageView(0);
    ImageView lastFrame = sequence.getImageView(99);
    
    EXPECT_EQ(firstFrame.getDataAsUint8()[0], 1);
    EXPECT_EQ(lastFrame.getDataAsUint8()[0], 1);
}

TEST_F(ImageSequenceTest, Float32EdgeValues) {
    // Arrange
    ImageSequence sequence(1, 1, 1, ImageDataType::FLOAT32);
    std::vector<float> specialValues = {0.0f}; // Single pixel
    
    // Test with special float values
    std::vector<std::vector<float>> testFrames = {
        {0.0f},                    // Zero
        {1.0f},                    // One
        {-1.0f},                   // Negative
        {std::numeric_limits<float>::max()},     // Max float
        {std::numeric_limits<float>::min()},     // Min positive float
        {std::numeric_limits<float>::infinity()}, // Infinity
        {-std::numeric_limits<float>::infinity()}, // Negative infinity
    };
    
    // Act & Assert - Should handle all these values without issues
    for (const auto& frame : testFrames) {
        sequence.addFrame(std::span<const float>(frame));
    }
    
    EXPECT_EQ(sequence.getFrameCount(), testFrames.size());
    
    // Verify values are preserved
    for (size_t i = 0; i < testFrames.size(); ++i) {
        ImageView view = sequence.getImageView(static_cast<uint32_t>(i));
        auto data = view.getDataAsFloat32();
        
        if (std::isfinite(testFrames[i][0])) {
            EXPECT_FLOAT_EQ(data[0], testFrames[i][0]);
        } else {
            EXPECT_EQ(data[0], testFrames[i][0]); // For infinity/NaN, use exact comparison
        }
    }
}

} // namespace thor::core::test 