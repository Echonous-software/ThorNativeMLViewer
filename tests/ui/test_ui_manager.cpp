#include <gtest/gtest.h>
#include <ui/UIManager.hpp>
#include <rendering/GLContext.hpp>
#include <rendering/GLRenderer.hpp>
#include <data/DataManager.hpp>
#include <core/Error.hpp>
#include <memory>
#include <GLFW/glfw3.h>

class UIManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize GLFW for window creation
        if (!glfwInit()) {
            FAIL() << "Failed to initialize GLFW";
        }
        
        // Create a hidden test window
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(800, 600, "UIManager Test", nullptr, nullptr);
        ASSERT_NE(window, nullptr) << "Failed to create GLFW window";
        
        glfwMakeContextCurrent(window);
        
        // Create UIManager and initialize
        uiManager = std::make_unique<thor::ui::UIManager>();
        ASSERT_TRUE(uiManager->initialize(window));
        
        // Set up test callbacks
        setupCallbacks();
    }
    
    void TearDown() override {
        uiManager.reset();
        
        if (window) {
            glfwDestroyWindow(window);
        }
        
        glfwTerminate();
    }
    
    void setupCallbacks() {
        // Track zoom callback invocations
        zoomCallbackCount = 0;
        lastZoomFactor = 0.0f;
        lastZoomToFit = false;
        
        uiManager->setZoomChangeCallback([this](float zoom, bool toFit) {
            zoomCallbackCount++;
            lastZoomFactor = zoom;
            lastZoomToFit = toFit;
        });
    }
    
    GLFWwindow* window = nullptr;
    std::unique_ptr<thor::ui::UIManager> uiManager;
    
    // Callback tracking
    int zoomCallbackCount = 0;
    float lastZoomFactor = 0.0f;
    bool lastZoomToFit = false;
};

// Test zoom state initialization
TEST_F(UIManagerTest, ZoomStateInitialization) {
    const auto& state = uiManager->getUIState();
    EXPECT_FLOAT_EQ(state.zoomFactor, 1.0f);
    EXPECT_TRUE(state.isZoomToFit);
    EXPECT_FLOAT_EQ(state.zoomMin, 0.1f);
    EXPECT_FLOAT_EQ(state.zoomMax, 10.0f);
}

// Test zoom in functionality
TEST_F(UIManagerTest, ZoomIn) {
    // Verify initial state
    EXPECT_TRUE(uiManager->getUIState().isZoomToFit);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 1.0f);
    
    // Start with manual zoom mode
    uiManager->setZoom(1.0f);
    EXPECT_EQ(zoomCallbackCount, 1); // Should trigger callback due to zoom-to-fit change
    
    // Zoom in
    uiManager->zoomIn();
    
    const auto& state = uiManager->getUIState();
    EXPECT_FLOAT_EQ(state.zoomFactor, 1.25f);
    EXPECT_FALSE(state.isZoomToFit);
    EXPECT_EQ(zoomCallbackCount, 2); // One for setZoom, one for zoomIn
    EXPECT_FLOAT_EQ(lastZoomFactor, 1.25f);
    EXPECT_FALSE(lastZoomToFit);
}

// Test zoom out functionality
TEST_F(UIManagerTest, ZoomOut) {
    // Start with manual zoom mode
    uiManager->setZoom(2.0f);
    
    // Zoom out
    uiManager->zoomOut();
    
    const auto& state = uiManager->getUIState();
    EXPECT_FLOAT_EQ(state.zoomFactor, 1.6f);
    EXPECT_FALSE(state.isZoomToFit);
    EXPECT_EQ(zoomCallbackCount, 2); // One for setZoom, one for zoomOut
    EXPECT_FLOAT_EQ(lastZoomFactor, 1.6f);
    EXPECT_FALSE(lastZoomToFit);
}

// Test zoom to fit functionality
TEST_F(UIManagerTest, ZoomToFit) {
    // Start with manual zoom mode
    uiManager->setZoom(2.0f);
    EXPECT_FALSE(uiManager->getUIState().isZoomToFit);
    
    // Zoom to fit
    uiManager->zoomToFit();
    
    const auto& state = uiManager->getUIState();
    EXPECT_TRUE(state.isZoomToFit);
    EXPECT_EQ(zoomCallbackCount, 2); // One for setZoom, one for zoomToFit
    EXPECT_TRUE(lastZoomToFit);
}

// Test zoom limits - maximum
TEST_F(UIManagerTest, ZoomMaxLimit) {
    // Set zoom near maximum
    uiManager->setZoom(9.8f);
    
    // Try to zoom in beyond maximum
    uiManager->zoomIn();
    
    const auto& state = uiManager->getUIState();
    EXPECT_FLOAT_EQ(state.zoomFactor, 10.0f); // Should be clamped to max
    EXPECT_FALSE(state.isZoomToFit);
    
    // Try to zoom in again - should not change
    int previousCallbackCount = zoomCallbackCount;
    uiManager->zoomIn();
    EXPECT_EQ(zoomCallbackCount, previousCallbackCount); // No callback should fire
    EXPECT_FLOAT_EQ(state.zoomFactor, 10.0f); // Should remain at max
}

// Test zoom limits - minimum
TEST_F(UIManagerTest, ZoomMinLimit) {
    // Set zoom near minimum
    uiManager->setZoom(0.12f);
    
    // Try to zoom out beyond minimum
    uiManager->zoomOut();
    
    const auto& state = uiManager->getUIState();
    EXPECT_FLOAT_EQ(state.zoomFactor, 0.1f); // Should be clamped to min
    EXPECT_FALSE(state.isZoomToFit);
    
    // Try to zoom out again - should not change
    int previousCallbackCount = zoomCallbackCount;
    uiManager->zoomOut();
    EXPECT_EQ(zoomCallbackCount, previousCallbackCount); // No callback should fire
    EXPECT_FLOAT_EQ(state.zoomFactor, 0.1f); // Should remain at min
}

// Test setZoom with clamping
TEST_F(UIManagerTest, SetZoomClamping) {
    // Test setting zoom above maximum
    uiManager->setZoom(15.0f);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 10.0f);
    
    // Test setting zoom below minimum
    uiManager->setZoom(0.05f);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 0.1f);
    
    // Test setting valid zoom
    uiManager->setZoom(2.5f);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 2.5f);
    EXPECT_FALSE(uiManager->getUIState().isZoomToFit);
}

// Test mouse wheel handling
TEST_F(UIManagerTest, MouseWheelZoom) {
    // Start with manual zoom mode
    uiManager->setZoom(1.0f);
    
    // Positive wheel (zoom in)
    uiManager->handleMouseWheel(1.0f);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 1.25f);
    
    // Negative wheel (zoom out)
    uiManager->handleMouseWheel(-1.0f);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 1.0f);
    
    // Zero wheel (no change)
    int previousCallbackCount = zoomCallbackCount;
    uiManager->handleMouseWheel(0.0f);
    EXPECT_EQ(zoomCallbackCount, previousCallbackCount);
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, 1.0f);
}

// Test zoom callback parameters
TEST_F(UIManagerTest, ZoomCallbackParameters) {
    // Test zoom to fit callback
    uiManager->zoomToFit();
    EXPECT_TRUE(lastZoomToFit);
    
    // Test manual zoom callback
    uiManager->setZoom(3.0f);
    EXPECT_FALSE(lastZoomToFit);
    EXPECT_FLOAT_EQ(lastZoomFactor, 3.0f);
}

// Test that zoom operations disable zoom to fit
TEST_F(UIManagerTest, ZoomOperationsDisableZoomToFit) {
    // Start in zoom to fit mode
    uiManager->zoomToFit();
    EXPECT_TRUE(uiManager->getUIState().isZoomToFit);
    
    // Any zoom operation should disable zoom to fit
    uiManager->zoomIn();
    EXPECT_FALSE(uiManager->getUIState().isZoomToFit);
    
    // Reset to zoom to fit
    uiManager->zoomToFit();
    EXPECT_TRUE(uiManager->getUIState().isZoomToFit);
    
    // Zoom out should also disable zoom to fit
    uiManager->zoomOut();
    EXPECT_FALSE(uiManager->getUIState().isZoomToFit);
    
    // Reset to zoom to fit
    uiManager->zoomToFit();
    EXPECT_TRUE(uiManager->getUIState().isZoomToFit);
    
    // setZoom should also disable zoom to fit
    uiManager->setZoom(2.0f);
    EXPECT_FALSE(uiManager->getUIState().isZoomToFit);
}

// Test zoom factor scaling
TEST_F(UIManagerTest, ZoomFactorScaling) {
    float baseZoom = 2.0f;
    uiManager->setZoom(baseZoom);
    
    uiManager->zoomIn();
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, baseZoom * 1.25f);
    
    uiManager->zoomOut();
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, baseZoom);
    
    uiManager->zoomOut();
    EXPECT_FLOAT_EQ(uiManager->getUIState().zoomFactor, baseZoom / 1.25f);
} 