#include <rendering/GLRenderer.hpp>
#include <core/Error.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>

namespace thor::rendering {

TransformMatrix TransformMatrix::operator*(const TransformMatrix& other) const {
    TransformMatrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += data[k * 4 + i] * other.data[j * 4 + k];
            }
            result.data[j * 4 + i] = sum;
        }
    }
    return result;
}

TransformMatrix TransformMatrix::inverse() const {
    TransformMatrix inv;
    const float* m = data;
    float* invOut = inv.data;

    invOut[0] = m[5]  * m[10] * m[15] - m[5]  * m[11] * m[14] - m[9]  * m[6] * m[15] + m[9]  * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    invOut[4] = -m[4]  * m[10] * m[15] + m[4]  * m[11] * m[14] + m[8]  * m[6] * m[15] - m[8]  * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    invOut[8] = m[4]  * m[9] * m[15] - m[4]  * m[11] * m[13] - m[8]  * m[5] * m[15] + m[8]  * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    invOut[12] = -m[4]  * m[9] * m[14] + m[4]  * m[10] * m[13] + m[8]  * m[5] * m[14] - m[8]  * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    invOut[1] = -m[1]  * m[10] * m[15] + m[1]  * m[11] * m[14] + m[9]  * m[2] * m[15] - m[9]  * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    invOut[5] = m[0]  * m[10] * m[15] - m[0]  * m[11] * m[14] - m[8]  * m[2] * m[15] + m[8]  * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    invOut[9] = -m[0]  * m[9] * m[15] + m[0]  * m[11] * m[13] + m[8]  * m[1] * m[15] - m[8]  * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    invOut[13] = m[0]  * m[9] * m[14] - m[0]  * m[10] * m[13] - m[8]  * m[1] * m[14] + m[8]  * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    invOut[2] = m[1]  * m[6] * m[15] - m[1]  * m[7] * m[14] - m[5]  * m[2] * m[15] + m[5]  * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    invOut[6] = -m[0]  * m[6] * m[15] + m[0]  * m[7] * m[14] + m[4]  * m[2] * m[15] - m[4]  * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    invOut[10] = m[0]  * m[5] * m[15] - m[0]  * m[7] * m[13] - m[4]  * m[1] * m[15] + m[4]  * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    invOut[14] = -m[0]  * m[5] * m[14] + m[0]  * m[6] * m[13] + m[4]  * m[1] * m[14] - m[4]  * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    invOut[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    invOut[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    invOut[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    invOut[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    float det = m[0] * invOut[0] + m[1] * invOut[4] + m[2] * invOut[8] + m[3] * invOut[12];
    if (det == 0) return TransformMatrix(); // Should not happen with well-formed matrices

    det = 1.0f / det;
    for (int i = 0; i < 16; i++) inv.data[i] = invOut[i] * det;

    return inv;
}

Vec4 TransformMatrix::operator*(const Vec4& v) const {
    return {
        data[0] * v.x + data[4] * v.y + data[8] * v.z + data[12] * v.w,
        data[1] * v.x + data[5] * v.y + data[9] * v.z + data[13] * v.w,
        data[2] * v.x + data[6] * v.y + data[10] * v.z + data[14] * v.w,
        data[3] * v.x + data[7] * v.y + data[11] * v.z + data[15] * v.w
    };
}

Vec2 TransformMatrix::transformPoint(float x, float y) const {
    Vec4 p = {x, y, 0.0f, 1.0f};
    Vec4 transformed = (*this) * p;
    if (transformed.w != 0) {
        return {transformed.x / transformed.w, transformed.y / transformed.w};
    }
    return {transformed.x, transformed.y};
}

TransformMatrix TransformMatrix::createWorldToScreen(int viewportWidth, int viewportHeight) {
    TransformMatrix m;
    // This is a simplified orthographic projection that maps world coordinates
    // (which are pixel-based in this app) to OpenGL's normalized device coordinates.
    // It effectively creates a 2D orthographic projection matrix.
    m.data[0] = 2.0f / viewportWidth;
    m.data[5] = -2.0f / viewportHeight; // Y is inverted in OpenGL
    m.data[12] = -1.0f;
    m.data[13] = 1.0f;
    m.data[15] = 1.0f;
    return m;
}

TransformMatrix TransformMatrix::createImageTransform(
    int imageWidth, int imageHeight,
    float zoomFactor, bool zoomToFit,
    int viewportWidth, int viewportHeight) {
    
    // Calculate aspect ratios
    float viewportAspect = static_cast<float>(viewportWidth) / viewportHeight;
    float imageAspect = static_cast<float>(imageWidth) / imageHeight;

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (zoomToFit) {
        if (viewportAspect > imageAspect) {
            // Letterbox (fit width)
            scaleX = imageAspect / viewportAspect;
        } else {
            // Pillarbox (fit height)
            scaleY = viewportAspect / imageAspect;
        }
    } else {
        // Apply manual zoom on top of the base scale
        scaleX *= zoomFactor;
        scaleY *= zoomFactor;
    }

    TransformMatrix m;
    // Model space is a unit quad (-0.5 to 0.5), so we scale it to image dimensions
    m.data[0] = static_cast<float>(imageWidth) * scaleX;
    m.data[5] = static_cast<float>(imageHeight) * scaleY;
    
    return m;
}

GLRenderer::GLRenderer()
    : mInitialized(false)
    , mShaderProgram(0)
    , mMinValueUniform(-1)
    , mMaxValueUniform(-1)
    , mTextureUniform(-1)
    , mChannelsUniform(-1)
    , mTransformUniform(-1)
    , mPositionAttribute(-1)
    , mTexCoordAttribute(-1)
    , mVAO(0)
    , mVBO(0)
    , mEBO(0)
    , mViewportWidth(0)
    , mViewportHeight(0) {
}

GLRenderer::~GLRenderer() {
    shutdown();
}

GLRenderer::GLRenderer(GLRenderer&& other) noexcept
    : mInitialized(other.mInitialized)
    , mShaderProgram(other.mShaderProgram)
    , mMinValueUniform(other.mMinValueUniform)
    , mMaxValueUniform(other.mMaxValueUniform)
    , mTextureUniform(other.mTextureUniform)
    , mChannelsUniform(other.mChannelsUniform)
    , mTransformUniform(other.mTransformUniform)
    , mPositionAttribute(other.mPositionAttribute)
    , mTexCoordAttribute(other.mTexCoordAttribute)
    , mVAO(other.mVAO)
    , mVBO(other.mVBO)
    , mEBO(other.mEBO)
    , mViewportWidth(other.mViewportWidth)
    , mViewportHeight(other.mViewportHeight) {
    
    // Reset other object
    other.mInitialized = false;
    other.mShaderProgram = 0;
    other.mVAO = 0;
    other.mVBO = 0;
    other.mEBO = 0;
}

GLRenderer& GLRenderer::operator=(GLRenderer&& other) noexcept {
    if (this != &other) {
        shutdown();
        
        mInitialized = other.mInitialized;
        mShaderProgram = other.mShaderProgram;
        mMinValueUniform = other.mMinValueUniform;
        mMaxValueUniform = other.mMaxValueUniform;
        mTextureUniform = other.mTextureUniform;
        mChannelsUniform = other.mChannelsUniform;
        mTransformUniform = other.mTransformUniform;
        mPositionAttribute = other.mPositionAttribute;
        mTexCoordAttribute = other.mTexCoordAttribute;
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEBO = other.mEBO;
        mViewportWidth = other.mViewportWidth;
        mViewportHeight = other.mViewportHeight;
        
        // Reset other object
        other.mInitialized = false;
        other.mShaderProgram = 0;
        other.mVAO = 0;
        other.mVBO = 0;
        other.mEBO = 0;
    }
    return *this;
}

bool GLRenderer::initialize() {
    if (mInitialized) {
        return true;
    }
    
    try {
        // Get current viewport
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        mViewportWidth = viewport[2];
        mViewportHeight = viewport[3];
        
        // Create shader program
        if (!createShaderProgram()) {
            throw thor::core::OpenGLError("Failed to create shader program");
        }
        
        // Create vertex buffers
        if (!createVertexBuffers()) {
            destroyShaderProgram();
            throw thor::core::OpenGLError("Failed to create vertex buffers");
        }
        
        // Default rendering parameters no longer stored as member variables
        
        mInitialized = true;
        return true;
        
    } catch (const std::exception& e) {
        releaseResources();
        std::cerr << "GLRenderer initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void GLRenderer::shutdown() {
    if (mInitialized) {
        releaseResources();
        mInitialized = false;
    }
}

TextureID GLRenderer::createTexture(const thor::data::ImageView& imageView) {
    validateInitialized();
    
    uint32_t textureId;
    glGenTextures(1, &textureId);
    
    if (textureId == 0) {
        throw thor::core::OpenGLError("Failed to generate texture");
    }
    
    try {
        updateTexture(textureId, imageView);
        return textureId;
        
    } catch (const std::exception& e) {
        glDeleteTextures(1, &textureId);
        throw;
    }
}

void GLRenderer::updateTexture(TextureID textureId, const thor::data::ImageView& imageView) {
    validateInitialized();
    
    if (textureId == INVALID_TEXTURE_ID) {
        throw thor::core::OpenGLError("Invalid texture ID: " + std::to_string(textureId));
    }
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Get OpenGL format parameters
    uint32_t internalFormat = getOpenGLInternalFormat(imageView.getPixelType(), imageView.getChannels());
    uint32_t format = getOpenGLFormat(imageView.getChannels());
    uint32_t type = getOpenGLType(imageView.getPixelType());
    
    // Get pixel data
    const void* pixelData = nullptr;
    if (imageView.getPixelType() == thor::data::ImageDataType::UINT8) {
        auto data = imageView.getDataAsUint8();
        pixelData = data.data();
    } else {
        auto data = imageView.getDataAsFloat32();
        pixelData = data.data();
    }
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 
                 imageView.getWidth(), imageView.getHeight(), 0,
                 format, type, pixelData);
    
    // Set texture parameters
    bindTextureParameters();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        throw thor::core::OpenGLError("Failed to update texture: OpenGL error " + std::to_string(error));
    }
}

void GLRenderer::deleteTexture(TextureID textureId) {
    if (textureId != INVALID_TEXTURE_ID) {
        glDeleteTextures(1, &textureId);
    }
}









void GLRenderer::renderQuadAt(TextureID textureId, const TransformMatrix& transform, const RenderingParameters& params) {
    validateInitialized();
    validateTextureId(textureId);

    glUseProgram(mShaderProgram);

    // Set rendering parameters
    glUniform1f(mMinValueUniform, params.minValue);
    glUniform1f(mMaxValueUniform, params.maxValue);
    glUniform1i(mChannelsUniform, params.channels);

    // Set transformation matrix
    glUniformMatrix4fv(mTransformUniform, 1, GL_FALSE, transform.data);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(mTextureUniform, 0);

    // Draw the quad
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind
    glBindVertexArray(0);
    glUseProgram(0);
}




void GLRenderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
    mViewportWidth = width;
    mViewportHeight = height;
}

void GLRenderer::getViewport(int& width, int& height) const {
    width = mViewportWidth;
    height = mViewportHeight;
}

void GLRenderer::updateViewportFromGL() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    mViewportWidth = viewport[2];
    mViewportHeight = viewport[3];
}

bool GLRenderer::isValidTexture(TextureID textureId) const {
    return textureId != INVALID_TEXTURE_ID && glIsTexture(textureId);
}

void GLRenderer::getTextureInfo(TextureID textureId, int& width, int& height, int& channels) const {
    // This is a simplified implementation. A more robust solution would store this
    // information in a map alongside the texture ID.
    // For now, we'll leave it as a placeholder.
    width = 0;
    height = 0;
    channels = 0;
}

// Private methods
bool GLRenderer::createShaderProgram() {
    // Compile vertex shader
    uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, getVertexShaderSource());
    if (vertexShader == 0) return false;

    uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, getFragmentShaderSource());
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);

    if (!linkShaderProgram(mShaderProgram)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // Detach and delete shaders as they are no longer needed
    glDetachShader(mShaderProgram, vertexShader);
    glDetachShader(mShaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get uniform and attribute locations
    mMinValueUniform = glGetUniformLocation(mShaderProgram, "u_minValue");
    mMaxValueUniform = glGetUniformLocation(mShaderProgram, "u_maxValue");
    mTextureUniform = glGetUniformLocation(mShaderProgram, "u_texture");
    mChannelsUniform = glGetUniformLocation(mShaderProgram, "u_channels");
    mTransformUniform = glGetUniformLocation(mShaderProgram, "u_transform");
    mPositionAttribute = glGetAttribLocation(mShaderProgram, "a_position");
    mTexCoordAttribute = glGetAttribLocation(mShaderProgram, "a_texCoord");

    return true;
}

void GLRenderer::destroyShaderProgram() {
    if (mShaderProgram != 0) {
        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0;
    }
}

uint32_t GLRenderer::compileShader(uint32_t type, const std::string& source) {
    uint32_t shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Shader compilation failed: " << getShaderError(shader) << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool GLRenderer::linkShaderProgram(uint32_t program) {
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Shader program linking failed: " << getProgramError(program) << std::endl;
        return false;
    }

    return true;
}

std::string GLRenderer::getShaderError(uint32_t shader) const {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        std::vector<char> infoLog(length);
        glGetShaderInfoLog(shader, length, nullptr, infoLog.data());
        return std::string(infoLog.begin(), infoLog.end());
    }
    return "Unknown shader error";
}

std::string GLRenderer::getProgramError(uint32_t program) const {
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
        std::vector<char> infoLog(length);
        glGetProgramInfoLog(program, length, nullptr, infoLog.data());
        return std::string(infoLog.begin(), infoLog.end());
    }
    return "Unknown program error";
}

bool GLRenderer::createVertexBuffers() {
    // Vertex data for a unit quad (model space coordinates)
    float vertices[] = {
        // positions         // texture coords
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);

    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(mPositionAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(mPositionAttribute);
    // Texture coord attribute
    glVertexAttribPointer(mTexCoordAttribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(mTexCoordAttribute);

    glBindVertexArray(0);

    return true;
}

void GLRenderer::destroyVertexBuffers() {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(1, &mEBO);
}

uint32_t GLRenderer::getOpenGLInternalFormat(thor::data::ImageDataType pixelType, uint32_t channels) const {
    if (pixelType == thor::data::ImageDataType::FLOAT32) {
        switch (channels) {
            case 1: return GL_R32F;
            case 3: return GL_RGB32F;
            case 4: return GL_RGBA32F;
        }
    } else { // UINT8
        switch (channels) {
            case 1: return GL_R8;
            case 3: return GL_RGB8;
            case 4: return GL_RGBA8;
        }
    }
    throw std::runtime_error("Unsupported image format");
}

uint32_t GLRenderer::getOpenGLFormat(uint32_t channels) const {
    switch (channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
    }
    throw std::runtime_error("Unsupported channel count");
}

uint32_t GLRenderer::getOpenGLType(thor::data::ImageDataType pixelType) const {
    switch (pixelType) {
        case thor::data::ImageDataType::UINT8: return GL_UNSIGNED_BYTE;
        case thor::data::ImageDataType::FLOAT32: return GL_FLOAT;
    }
    throw std::runtime_error("Unsupported pixel type");
}

void GLRenderer::bindTextureParameters() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

const char* GLRenderer::getVertexShaderSource() {
    return R"glsl(
#version 330 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texCoord;

uniform mat4 u_transform;

out vec2 v_texCoord;

void main() {
    gl_Position = u_transform * vec4(a_position, 1.0);
    v_texCoord = a_texCoord;
}
)glsl";
}

const char* GLRenderer::getFragmentShaderSource() {
    return R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform float u_minValue;
uniform float u_maxValue;
uniform int u_channels;

void main() {
    vec4 texColor = texture(u_texture, v_texCoord);
    float value = texColor.r; // Assume single channel for now

    if (u_channels == 1) {
        float normalizedValue = (value - u_minValue) / (u_maxValue - u_minValue);
        FragColor = vec4(vec3(normalizedValue), 1.0);
    } else {
        // Pass-through for multi-channel images (already normalized)
        FragColor = texColor;
    }
}
)glsl";
}

void GLRenderer::validateInitialized() const {
    if (!mInitialized) {
        throw std::runtime_error("GLRenderer is not initialized");
    }
}

void GLRenderer::validateTextureId(TextureID textureId) const {
    if (!isValidTexture(textureId)) {
        throw std::runtime_error("Invalid texture ID: " + std::to_string(textureId));
    }
}

void GLRenderer::releaseResources() {
    destroyShaderProgram();
    destroyVertexBuffers();
}

} // namespace thor::rendering 