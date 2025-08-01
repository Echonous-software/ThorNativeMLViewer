#include <gtest/gtest.h>
#include <rendering/GLRenderer.hpp>
#include <rendering/GLContext.hpp>
#include <data/ImageSequence.hpp>
#include <core/Error.hpp>
#include <glad/glad.h>
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
        
        // Create wide test image (8x4, RGB)
        testImageWide = std::make_unique<thor::data::ImageSequence>(8, 4, 3, thor::data::ImageDataType::UINT8);
        std::vector<uint8_t> wideData(8 * 4 * 3);
        for (size_t i = 0; i < wideData.size(); ++i) {
            wideData[i] = static_cast<uint8_t>((i * 30) % 256);
        }
        testImageWide->addFrame(wideData);
        
        // Create tall test image (4x8, RGB)
        testImageTall = std::make_unique<thor::data::ImageSequence>(4, 8, 3, thor::data::ImageDataType::UINT8);
        std::vector<uint8_t> tallData(4 * 8 * 3);
        for (size_t i = 0; i < tallData.size(); ++i) {
            tallData[i] = static_cast<uint8_t>((i * 40) % 256);
        }
        testImageTall->addFrame(tallData);
    }
    
    std::unique_ptr<thor::rendering::GLContext> glContext;
    std::unique_ptr<thor::rendering::GLRenderer> glRenderer;
    std::unique_ptr<thor::data::ImageSequence> testImageUint8;
    std::unique_ptr<thor::data::ImageSequence> testImageFloat32;
    std::unique_ptr<thor::data::ImageSequence> testImageGrayscale;
    
    // Test image for aspect ratio testing
    std::unique_ptr<thor::data::ImageSequence> testImageWide;   // 8x4 (wide)
    std::unique_ptr<thor::data::ImageSequence> testImageTall;   // 4x8 (tall)
};

// Helper function to create a basic transform matrix for testing
thor::rendering::TransformMatrix createBasicTransform(int imageWidth, int imageHeight, float zoom = 1.0f, bool zoomToFit = true, int vpWidth = 800, int vpHeight = 600) {
    return thor::rendering::TransformMatrix::createImageTransform(imageWidth, imageHeight, zoom, zoomToFit, vpWidth, vpHeight);
}

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

// Test matrix-based quad rendering
TEST_F(GLRendererTest, RenderQuadWithMatrix) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test rendering with default parameters
    thor::rendering::TransformMatrix transform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    thor::rendering::RenderingParameters defaultParams(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, defaultParams));
    
    // Test rendering with custom parameters
    thor::rendering::RenderingParameters params(1.5f, 0.1f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    // Test rendering with custom transform (zoomed)
    auto zoomTransform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 2.0f, false, 800, 600);
    thor::rendering::RenderingParameters zoomParams(0.9f, -0.05f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, zoomTransform, zoomParams));
    
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
    
    thor::rendering::TransformMatrix invalidTransform;
    thor::rendering::RenderingParameters invalidParams;
    EXPECT_THROW(glRenderer->renderQuadAt(invalidTextureId, invalidTransform, invalidParams),
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
    
    // Test rendering different textures with matrix-based API
    thor::rendering::TransformMatrix transform;
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId1, transform, thor::rendering::RenderingParameters(1.0f, 0.0f, 3)));
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId2, transform, thor::rendering::RenderingParameters(0.5f, 0.5f, 3)));
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId3, transform, thor::rendering::RenderingParameters(2.0f, -0.1f, 3)));
    
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
        thor::rendering::TransformMatrix transform;
        thor::rendering::RenderingParameters renderParams(minValue, maxValue, imageView.getChannels());
        EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, renderParams));
    }
    
    glRenderer->deleteTexture(textureId);
}

// Test centered rendering with zoom to fit
TEST_F(GLRendererTest, CenteredRenderingZoomToFit) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test zoom to fit rendering using matrix-based API
    auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test centered rendering with manual zoom
TEST_F(GLRendererTest, CenteredRenderingManualZoom) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test manual zoom rendering
    auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 2.0f, false, 800, 600);
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test aspect ratio preservation with wide image
TEST_F(GLRendererTest, AspectRatioPreservationWideImage) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageWide->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test aspect ratio preservation with tall image  
TEST_F(GLRendererTest, AspectRatioPreservationTallImage) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageTall->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test centered rendering with invalid parameters
TEST_F(GLRendererTest, CenteredRenderingInvalidParameters) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test with invalid texture ID
    thor::rendering::TransformMatrix transform;
    thor::rendering::RenderingParameters params;
    EXPECT_THROW(glRenderer->renderQuadAt(thor::rendering::INVALID_TEXTURE_ID, transform, params),
                 thor::core::OpenGLError);
    
    glRenderer->deleteTexture(textureId);
}

// Test centered rendering with custom parameters
TEST_F(GLRendererTest, CenteredRenderingWithParameters) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.5f, false, 800, 600);
    thor::rendering::RenderingParameters params(0.2f, 0.8f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test zoom boundaries
TEST_F(GLRendererTest, CenteredRenderingZoomBoundaries) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test minimum zoom
    auto minTransform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 0.1f, false, 800, 600);
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, minTransform, params));
    
    // Test maximum zoom  
    auto maxTransform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 10.0f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, maxTransform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test viewport update preserves aspect ratio
TEST_F(GLRendererTest, ViewportUpdatePreservesAspectRatio) {
    ASSERT_TRUE(glRenderer->initialize());
    
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test with different viewport sizes
    std::vector<std::pair<int, int>> viewportSizes = {{800, 600}, {1024, 768}, {1920, 1080}};
    
    for (const auto& [width, height] : viewportSizes) {
        glRenderer->setViewport(width, height);
        auto transform = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.0f, true, width, height);
        thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
        EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    }
    
    glRenderer->deleteTexture(textureId);
}

// Test centered rendering with explicit channels parameter
TEST_F(GLRendererTest, CenteredRenderingWithChannels) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    // Test with different channel configurations
    auto imageViewGrayscale = testImageGrayscale->getImageView(0);
    auto imageViewRGB = testImageUint8->getImageView(0);
    
    auto textureIdGray = glRenderer->createTexture(imageViewGrayscale);
    auto textureIdRGB = glRenderer->createTexture(imageViewRGB);
    
    // Test grayscale image (1 channel)
    thor::rendering::RenderingParameters paramsGray(0.0f, 1.0f, imageViewGrayscale.getChannels());
    auto transformGray = createBasicTransform(imageViewGrayscale.getWidth(), imageViewGrayscale.getHeight(), 1.0f, true, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureIdGray, transformGray, paramsGray));
    
    // Test RGB image (3 channels)
    thor::rendering::RenderingParameters paramsRGB(0.0f, 1.0f, imageViewRGB.getChannels());
    auto transformRGB = createBasicTransform(imageViewRGB.getWidth(), imageViewRGB.getHeight(), 2.0f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureIdRGB, transformRGB, paramsRGB));
    
    // Test with rendering parameters
    thor::rendering::RenderingParameters params(0.1f, 0.9f, imageViewRGB.getChannels());
    auto transformParams = createBasicTransform(imageViewRGB.getWidth(), imageViewRGB.getHeight(), 1.5f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureIdRGB, transformParams, params));
    
    glRenderer->deleteTexture(textureIdGray);
    glRenderer->deleteTexture(textureIdRGB);
} 

// Test consolidated RenderingParameters structure
TEST_F(GLRendererTest, ConsolidatedRenderingParameters) {
    ASSERT_TRUE(glRenderer->initialize());
    
    // Test default constructor
    thor::rendering::RenderingParameters defaultParams;
    EXPECT_FLOAT_EQ(defaultParams.minValue, 0.0f);
    EXPECT_FLOAT_EQ(defaultParams.maxValue, 1.0f);
    EXPECT_EQ(defaultParams.channels, 3);
    
    // Test min/max constructor
    thor::rendering::RenderingParameters minMaxParams(0.2f, 0.8f);
    EXPECT_FLOAT_EQ(minMaxParams.minValue, 0.2f);
    EXPECT_FLOAT_EQ(minMaxParams.maxValue, 0.8f);
    EXPECT_EQ(minMaxParams.channels, 3);  // Should default to 3
    
    // Test full constructor with channels
    thor::rendering::RenderingParameters fullParams(0.1f, 0.9f, 1);
    EXPECT_FLOAT_EQ(fullParams.minValue, 0.1f);
    EXPECT_FLOAT_EQ(fullParams.maxValue, 0.9f);
    EXPECT_EQ(fullParams.channels, 1);
    
    // Test that params work correctly in rendering
    auto imageView = testImageGrayscale->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    thor::rendering::TransformMatrix transform;
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, fullParams));
    
    glRenderer->deleteTexture(textureId);
} 

// Test static unit quad VBO implementation  
TEST_F(GLRendererTest, StaticUnitQuadVBO) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    // Create a texture for testing
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Verify that multiple rendering calls work with matrix-based API
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    
    // Multiple renders with different transform matrices should work without VBO updates
    auto transform1 = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform1, params));
        
    auto transform2 = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 2.0f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform2, params));
        
    auto transform3 = createBasicTransform(imageView.getWidth(), imageView.getHeight(), 0.5f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform3, params));
    
    // Test with identity transform as well
    thor::rendering::TransformMatrix identityTransform;
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, identityTransform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test matrix-based rendering pipeline
TEST_F(GLRendererTest, MatrixBasedRenderQuadAt) {
    ASSERT_TRUE(glRenderer->initialize());
    
    glRenderer->setViewport(800, 600);
    
    // Create a test texture
    auto imageView = testImageUint8->getImageView(0);
    auto textureId = glRenderer->createTexture(imageView);
    
    // Test basic matrix transformation
    thor::rendering::TransformMatrix transform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 1.0f, true, 800, 600);
    
    thor::rendering::RenderingParameters params(0.0f, 1.0f, imageView.getChannels());
    
    // Test matrix-based rendering
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    // Test with different zoom levels
    transform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 2.0f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    transform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 0.5f, false, 800, 600);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    // Test with different viewport sizes
    transform = thor::rendering::TransformMatrix::createImageTransform(
        imageView.getWidth(), imageView.getHeight(), 1.0f, true, 1024, 768);
    EXPECT_NO_THROW(glRenderer->renderQuadAt(textureId, transform, params));
    
    glRenderer->deleteTexture(textureId);
}

// Test TransformMatrix creation methods
TEST_F(GLRendererTest, TransformMatrixCreation) {
    // Test identity matrix
    thor::rendering::TransformMatrix identity;
    EXPECT_FLOAT_EQ(identity.data[0], 1.0f);   // Scale X
    EXPECT_FLOAT_EQ(identity.data[5], 1.0f);   // Scale Y
    EXPECT_FLOAT_EQ(identity.data[10], 1.0f);  // Scale Z
    EXPECT_FLOAT_EQ(identity.data[15], 1.0f);  // W
    EXPECT_FLOAT_EQ(identity.data[12], 0.0f);  // Translation X
    EXPECT_FLOAT_EQ(identity.data[13], 0.0f);  // Translation Y
    
    // Test world to screen transformation
    auto worldToScreen = thor::rendering::TransformMatrix::createWorldToScreen(800, 600);
    
    // Verify the matrix has reasonable transformation values
    EXPECT_NE(worldToScreen.data[0], 0.0f);
    EXPECT_NE(worldToScreen.data[5], 0.0f);
    
    // Test image transformation
    auto imageTransform = thor::rendering::TransformMatrix::createImageTransform(
        256, 256, 1.0f, true, 800, 600);
    
    // Verify the matrix has reasonable transformation values
    EXPECT_GT(imageTransform.data[0], 0.0f);  // Scale X should be positive
    EXPECT_GT(imageTransform.data[5], 0.0f);  // Scale Y should be positive
}

TEST_F(GLRendererTest, TransformMatrixInversionAndPointTransform) {
    auto world_to_screen = thor::rendering::TransformMatrix::createWorldToScreen(800, 600);
    auto image_transform = thor::rendering::TransformMatrix::createImageTransform(256, 256, 1.0f, true, 800, 600);

    auto final_transform = world_to_screen * image_transform;
    auto inverse_transform = final_transform.inverse();

    // Test a point at the center of the screen
    float sx = 0.0f, sy = 0.0f;
    auto image_pos = inverse_transform.transformPoint(sx, sy);
    image_pos.x += 0.5f;
    image_pos.y += 0.5f;

    // Should be near the center of the image
    EXPECT_NEAR(image_pos.x, 0.5f, 1e-5);
    EXPECT_NEAR(image_pos.y, 0.5f, 1e-5);
} 