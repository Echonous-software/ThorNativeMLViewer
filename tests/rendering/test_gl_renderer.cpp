#include <gtest/gtest.h>
#include <rendering/GLRenderer.hpp>
#include <rendering/GLContext.hpp>
#include <data/ImageSequence.hpp>
#include <core/Error.hpp>
#include <memory>
#include <vector>

class GLRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize OpenGL context for testing
        glContext = std::make_unique<thor::rendering::GLContext>();
        ASSERT_TRUE(glContext->initialize(800, 600, "GLRenderer Test"));
        
        // Create GLRenderer
        glRenderer = std::make_unique<thor::rendering::GLRenderer>();
        
        // Create test image data
        createTestImageData();
    }
    
    void TearDown() override {
        glRenderer.reset();
        glContext.reset();
    }
    
    void createTestImageData() {
        // Create UINT8 test image (4x4, RGB)
        testImageUint8 = std::make_unique<thor::data::ImageSequence>(4, 4, 3, thor::data::ImageDataType::UINT8);
        std::vector<uint8_t> pixelData(4 * 4 * 3);
        for (size_t i = 0; i < pixelData.size(); ++i) {
            pixelData[i] = static_cast<uint8_t>((i * 50) % 256);
        }
        testImageUint8->addFrame(pixelData);
        
        // Create FLOAT32 test image (4x4, RGB)
        testImageFloat32 = std::make_unique<thor::data::ImageSequence>(4, 4, 3, thor::data::ImageDataType::FLOAT32);
        std::vector<float> floatData(4 * 4 * 3);
        for (size_t i = 0; i < floatData.size(); ++i) {
            floatData[i] = static_cast<float>(i) * 0.1f;
        }
        testImageFloat32->addFrame(floatData);
        
        // Create single channel test image (4x4, grayscale)
        testImageGrayscale = std::make_unique<thor::data::ImageSequence>(4, 4, 1, thor::data::ImageDataType::UINT8);
        std::vector<uint8_t> grayData(4 * 4);
        for (size_t i = 0; i < grayData.size(); ++i) {
            grayData[i] = static_cast<uint8_t>(i * 16);
        }
        testImageGrayscale->addFrame(grayData);
    }
    
    std::unique_ptr<thor::rendering::GLContext> glContext;
    std::unique_ptr<thor::rendering::GLRenderer> glRenderer;
    std::unique_ptr<thor::data::ImageSequence> testImageUint8;
    std::unique_ptr<thor::data::ImageSequence> testImageFloat32;
    std::unique_ptr<thor::data::ImageSequence> testImageGrayscale;
};

// Test basic initialization
TEST_F(GLRendererTest, Initialization) {
    EXPECT_FALSE(glRenderer->isInitialized());
    
    EXPECT_TRUE(glRenderer->initialize());
    EXPECT_TRUE(glRenderer->isInitialized());
    
    // Should not fail to initialize again
    EXPECT_TRUE(glRenderer->initialize());
    
    glRenderer->shutdown();
    EXPECT_FALSE(glRenderer->isInitialized());
}

// Test texture creation with UINT8 data
TEST_F(GLRendererTest, CreateTextureUint8) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    EXPECT_NE(textureId, thor::rendering::INVALID_TEXTURE_ID);
    EXPECT_TRUE(glRenderer->isValidTexture(textureId));
    
    // Verify texture properties
    int width, height, channels;
    glRenderer->getTextureInfo(textureId, width, height, channels);
    EXPECT_EQ(width, 4);
    EXPECT_EQ(height, 4);
    EXPECT_EQ(channels, 3);
    
    glRenderer->deleteTexture(textureId);
    EXPECT_FALSE(glRenderer->isValidTexture(textureId));
}

// Test texture creation with FLOAT32 data
TEST_F(GLRendererTest, CreateTextureFloat32) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageFloat32->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    EXPECT_NE(textureId, thor::rendering::INVALID_TEXTURE_ID);
    EXPECT_TRUE(glRenderer->isValidTexture(textureId));
    
    // Verify texture properties
    int width, height, channels;
    glRenderer->getTextureInfo(textureId, width, height, channels);
    EXPECT_EQ(width, 4);
    EXPECT_EQ(height, 4);
    EXPECT_EQ(channels, 3);
    
    glRenderer->deleteTexture(textureId);
}

// Test texture creation with single channel (grayscale)
TEST_F(GLRendererTest, CreateTextureGrayscale) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageGrayscale->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    EXPECT_NE(textureId, thor::rendering::INVALID_TEXTURE_ID);
    EXPECT_TRUE(glRenderer->isValidTexture(textureId));
    
    // Verify texture properties
    int width, height, channels;
    glRenderer->getTextureInfo(textureId, width, height, channels);
    EXPECT_EQ(width, 4);
    EXPECT_EQ(height, 4);
    EXPECT_EQ(channels, 1);
    
    glRenderer->deleteTexture(textureId);
}

// Test texture update
TEST_F(GLRendererTest, UpdateTexture) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Update with different data
    auto newImageView = testImageFloat32->getImageView(0);
    EXPECT_NO_THROW(glRenderer->updateTexture(textureId, newImageView));
    
    glRenderer->deleteTexture(textureId);
}

// Test rendering parameters
TEST_F(GLRendererTest, RenderingParameters) {
    ASSERT_TRUE(glRenderer->initialize());
    
    // Test default parameters
    auto defaultParams = glRenderer->getRenderingParameters();
    EXPECT_FLOAT_EQ(defaultParams.minValue, 0.0f);
    EXPECT_FLOAT_EQ(defaultParams.maxValue, 1.0f);
    
    // Test setting parameters via struct
    thor::rendering::RenderingParameters params(0.0f, 255.0f);
    glRenderer->setRenderingParameters(params);
    
    auto retrievedParams = glRenderer->getRenderingParameters();
    EXPECT_FLOAT_EQ(retrievedParams.minValue, 0.0f);
    EXPECT_FLOAT_EQ(retrievedParams.maxValue, 255.0f);
    
    // Test setting parameters via individual values
    glRenderer->setRenderingParameters(-10.0f, 100.0f);
    
    auto finalParams = glRenderer->getRenderingParameters();
    EXPECT_FLOAT_EQ(finalParams.minValue, -10.0f);
    EXPECT_FLOAT_EQ(finalParams.maxValue, 100.0f);
}

// Test viewport management
TEST_F(GLRendererTest, ViewportManagement) {
    ASSERT_TRUE(glRenderer->initialize());
    
    // Test setting viewport
    glRenderer->setViewport(1024, 768);
    
    int width, height;
    glRenderer->getViewport(width, height);
    EXPECT_EQ(width, 1024);
    EXPECT_EQ(height, 768);
    
    // Test different viewport size
    glRenderer->setViewport(640, 480);
    glRenderer->getViewport(width, height);
    EXPECT_EQ(width, 640);
    EXPECT_EQ(height, 480);
}

// Test rendering textured quad
TEST_F(GLRendererTest, RenderTexturedQuad) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test rendering with default parameters
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId));
    
    // Test rendering with custom parameters
    thor::rendering::RenderingParameters params(1.5f, 0.1f);
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId, params));
    
    // Test rendering with individual scale/bias values
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId, 0.9f, -0.05f));
    
    glRenderer->deleteTexture(textureId);
}

// Test error conditions
TEST_F(GLRendererTest, ErrorConditions) {
    // Test operations before initialization
    EXPECT_THROW(glRenderer->createTexture(testImageUint8->getImageView(0)), 
                 thor::core::OpenGLError);
    
    ASSERT_TRUE(glRenderer->initialize());
    
    // Test invalid texture operations
    auto invalidTextureId = thor::rendering::INVALID_TEXTURE_ID;
    EXPECT_FALSE(glRenderer->isValidTexture(invalidTextureId));
    
    EXPECT_THROW(glRenderer->updateTexture(invalidTextureId, testImageUint8->getImageView(0)),
                 thor::core::OpenGLError);
    
    EXPECT_THROW(glRenderer->renderTexturedQuad(invalidTextureId),
                 thor::core::OpenGLError);
    
    int dummyWidth, dummyHeight, dummyChannels;
    EXPECT_THROW(glRenderer->getTextureInfo(invalidTextureId, dummyWidth, dummyHeight, dummyChannels),
                 thor::core::OpenGLError);
}

// Test resource cleanup
TEST_F(GLRendererTest, ResourceCleanup) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    EXPECT_TRUE(glRenderer->isValidTexture(textureId));
    
    // Test shutdown cleans up resources properly
    glRenderer->shutdown();
    EXPECT_FALSE(glRenderer->isInitialized());
    
    // Should be able to initialize again
    EXPECT_TRUE(glRenderer->initialize());
}

// Test move semantics
TEST_F(GLRendererTest, MoveSemantics) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test move constructor
    thor::rendering::GLRenderer movedRenderer = std::move(*glRenderer);
    EXPECT_TRUE(movedRenderer.isInitialized());
    EXPECT_TRUE(movedRenderer.isValidTexture(textureId));
    
    // Original should be in moved-from state
    EXPECT_FALSE(glRenderer->isInitialized());
    
    movedRenderer.deleteTexture(textureId);
    movedRenderer.shutdown();
}

// Test multiple textures
TEST_F(GLRendererTest, MultipleTextures) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView1 = testImageUint8->getImageView(0);
    auto imageView2 = testImageFloat32->getImageView(0);
    auto imageView3 = testImageGrayscale->getImageView(0);
    
    auto textureId1 = glRenderer->createTexture(imageView1);
    auto textureId2 = glRenderer->createTexture(imageView2);
    auto textureId3 = glRenderer->createTexture(imageView3);
    
    // All should be valid and different
    EXPECT_TRUE(glRenderer->isValidTexture(textureId1));
    EXPECT_TRUE(glRenderer->isValidTexture(textureId2));
    EXPECT_TRUE(glRenderer->isValidTexture(textureId3));
    
    EXPECT_NE(textureId1, textureId2);
    EXPECT_NE(textureId2, textureId3);
    EXPECT_NE(textureId1, textureId3);
    
    // Test rendering different textures
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId1, 1.0f, 0.0f));
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId2, 0.5f, 0.5f));
    EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId3, 2.0f, -0.1f));
    
    glRenderer->deleteTexture(textureId1);
    glRenderer->deleteTexture(textureId2);
    glRenderer->deleteTexture(textureId3);
}

// Test shader-based scale and bias processing
TEST_F(GLRendererTest, ShaderMinMaxProcessing) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageFloat32->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test various min/max range combinations that would be used in ML visualization
    std::vector<std::pair<float, float>> testParams = {
        {0.0f, 1.0f},     // Identity range [0,1]
        {0.0f, 255.0f},   // Typical uint8 range [0,255]
        {-1.0f, 1.0f},    // Normalized range [-1,1]
        {50.0f, 200.0f},  // Custom range
        {-10.0f, 100.0f}  // Extended range
    };
    
    for (const auto& [minValue, maxValue] : testParams) {
        EXPECT_NO_THROW(glRenderer->renderTexturedQuad(textureId, minValue, maxValue));
        
        // Verify parameters are set correctly
        glRenderer->setRenderingParameters(minValue, maxValue);
        auto params = glRenderer->getRenderingParameters();
        EXPECT_FLOAT_EQ(params.minValue, minValue);
        EXPECT_FLOAT_EQ(params.maxValue, maxValue);
    }
    
    glRenderer->deleteTexture(textureId);
} 