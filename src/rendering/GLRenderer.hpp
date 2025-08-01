#pragma once

#include <data/ImageSequence.hpp>
#include <cstdint>
#include <memory>
#include <string>

// Forward declarations to avoid including OpenGL headers directly
struct GLFWwindow;

namespace thor::rendering {

// Texture handle type for OpenGL texture IDs
using TextureID = uint32_t;
constexpr TextureID INVALID_TEXTURE_ID = 0;

// Rendering parameters for shader uniforms
struct RenderingParameters {
    float minValue = 0.0f;
    float maxValue = 1.0f;
    uint32_t channels = 3;

    RenderingParameters() = default;
    RenderingParameters(float min, float max) : minValue(min), maxValue(max) {}
    RenderingParameters(float min, float max, uint32_t ch) : minValue(min), maxValue(max), channels(ch) {}
};

// Transformation matrix for coordinate space conversions
struct TransformMatrix {
    float data[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,  // Column 0
        0.0f, 1.0f, 0.0f, 0.0f,  // Column 1
        0.0f, 0.0f, 1.0f, 0.0f,  // Column 2
        0.0f, 0.0f, 0.0f, 1.0f   // Column 3 (identity matrix)
    };

    TransformMatrix() = default;
    
    // Create transformation matrix from world space position and scale
    static TransformMatrix createWorldToScreen(
        float worldX, float worldY,           // Position in world space
        float scaleX, float scaleY,           // Scale factors
        int viewportWidth, int viewportHeight // Viewport dimensions for screen space conversion
    );
    
    // Create transformation for image centering with zoom
    static TransformMatrix createImageTransform(
        int imageWidth, int imageHeight,      // Image dimensions
        float zoomFactor, bool zoomToFit,     // Zoom parameters
        int viewportWidth, int viewportHeight // Viewport dimensions
    );
};

// OpenGL renderer for texture management and shader-based image processing
class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();
    
    // Disable copy (manages OpenGL resources)
    GLRenderer(const GLRenderer&) = delete;
    GLRenderer& operator=(const GLRenderer&) = delete;
    
    // Enable move
    GLRenderer(GLRenderer&&) noexcept;
    GLRenderer& operator=(GLRenderer&&) noexcept;
    
    // Initialization and cleanup
    bool initialize();
    void shutdown();
    bool isInitialized() const { return mInitialized; }
    
    // Texture management
    TextureID createTexture(const thor::data::ImageView& imageView);
    void updateTexture(TextureID textureId, const thor::data::ImageView& imageView);
    void deleteTexture(TextureID textureId);
    
    // Modern matrix-based rendering
    void renderQuadAt(TextureID textureId, const TransformMatrix& transform, 
                     const RenderingParameters& params = {});
    
    // Viewport management
    void setViewport(int width, int height);
    void getViewport(int& width, int& height) const;
    void updateViewportFromGL(); // Update stored viewport from current OpenGL state
    
    // Resource queries
    bool isValidTexture(TextureID textureId) const;
    void getTextureInfo(TextureID textureId, int& width, int& height, int& channels) const;
    
private:
    // OpenGL state
    bool mInitialized;
    
    // Shader program and uniforms
    uint32_t mShaderProgram;
    int mMinValueUniform;
    int mMaxValueUniform;
    int mTextureUniform;
    int mChannelsUniform;
    int mTransformUniform;     // Model-to-screen transformation matrix
    int mPositionAttribute;
    int mTexCoordAttribute;
    
    // Vertex data for textured quad
    uint32_t mVAO;  // Vertex Array Object
    uint32_t mVBO;  // Vertex Buffer Object
    uint32_t mEBO;  // Element Buffer Object
    
    // Viewport dimensions
    int mViewportWidth;
    int mViewportHeight;
    
    // Shader compilation and management
    bool createShaderProgram();
    void destroyShaderProgram();
    uint32_t compileShader(uint32_t type, const std::string& source);
    bool linkShaderProgram(uint32_t program);
    std::string getShaderError(uint32_t shader) const;
    std::string getProgramError(uint32_t program) const;
    
    // Vertex buffer setup
    bool createVertexBuffers();
    void destroyVertexBuffers();
    
    // Texture utilities
    uint32_t getOpenGLInternalFormat(thor::data::ImageDataType pixelType, uint32_t channels) const;
    uint32_t getOpenGLFormat(uint32_t channels) const;
    uint32_t getOpenGLType(thor::data::ImageDataType pixelType) const;
    void bindTextureParameters();
    
    // Shader source code
    static const char* getVertexShaderSource();
    static const char* getFragmentShaderSource();
    
    // Resource validation
    void validateInitialized() const;
    void validateTextureId(TextureID textureId) const;
    
    // Cleanup helpers
    void releaseResources();
};

} // namespace thor::rendering 