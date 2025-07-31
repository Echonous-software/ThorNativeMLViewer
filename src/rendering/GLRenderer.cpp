#include <rendering/GLRenderer.hpp>
#include <core/Error.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

namespace thor::rendering {

GLRenderer::GLRenderer()
    : mInitialized(false)
    , mShaderProgram(0)
    , mMinValueUniform(-1)
    , mMaxValueUniform(-1)
    , mTextureUniform(-1)
    , mChannelsUniform(-1)
    , mPositionAttribute(-1)
    , mTexCoordAttribute(-1)
    , mVAO(0)
    , mVBO(0)
    , mEBO(0)
    , mRenderingParams()
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
    , mPositionAttribute(other.mPositionAttribute)
    , mTexCoordAttribute(other.mTexCoordAttribute)
    , mVAO(other.mVAO)
    , mVBO(other.mVBO)
    , mEBO(other.mEBO)
    , mRenderingParams(other.mRenderingParams)
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
        mPositionAttribute = other.mPositionAttribute;
        mTexCoordAttribute = other.mTexCoordAttribute;
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        mEBO = other.mEBO;
        mRenderingParams = other.mRenderingParams;
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
        
        // Set default rendering parameters
        setRenderingParameters(0.0f, 1.0f);
        
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

void GLRenderer::renderTexturedQuad(TextureID textureId, const RenderingParameters& params) {
    validateInitialized();
    
    if (textureId == INVALID_TEXTURE_ID) {
        throw thor::core::OpenGLError("Invalid texture ID: " + std::to_string(textureId));
    }
    
    // Use shader program
    glUseProgram(mShaderProgram);
    
    // Set uniforms
    glUniform1f(mMinValueUniform, params.minValue);
    glUniform1f(mMaxValueUniform, params.maxValue);
    glUniform1i(mTextureUniform, 0);  // Use texture unit 0
    glUniform1i(mChannelsUniform, 3);  // Default to 3 channels (RGB)
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Bind vertex array
    glBindVertexArray(mVAO);
    
    // Draw quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        throw thor::core::OpenGLError("Failed to render textured quad: OpenGL error " + std::to_string(error));
    }
}

void GLRenderer::renderTexturedQuad(TextureID textureId, float minValue, float maxValue) {
    renderTexturedQuad(textureId, RenderingParameters(minValue, maxValue));
}

void GLRenderer::renderTexturedQuad(TextureID textureId, uint32_t channels, const RenderingParameters& params) {
    validateInitialized();
    
    if (textureId == INVALID_TEXTURE_ID) {
        throw thor::core::OpenGLError("Invalid texture ID: " + std::to_string(textureId));
    }
    
    // Use shader program
    glUseProgram(mShaderProgram);
    
    // Set uniforms
    glUniform1f(mMinValueUniform, params.minValue);
    glUniform1f(mMaxValueUniform, params.maxValue);
    glUniform1i(mTextureUniform, 0);  // Use texture unit 0
    glUniform1i(mChannelsUniform, static_cast<int>(channels));  // Set actual channel count
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Bind vertex array and draw
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Unbind texture and shader
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        throw thor::core::OpenGLError("Failed to render textured quad: OpenGL error " + std::to_string(error));
    }
}

void GLRenderer::setRenderingParameters(const RenderingParameters& params) {
    mRenderingParams = params;
}

void GLRenderer::setRenderingParameters(float minValue, float maxValue) {
    mRenderingParams.minValue = minValue;
    mRenderingParams.maxValue = maxValue;
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

bool GLRenderer::isValidTexture(TextureID textureId) const {
    return textureId != INVALID_TEXTURE_ID && glIsTexture(textureId) == GL_TRUE;
}

void GLRenderer::getTextureInfo(TextureID textureId, int& width, int& height, int& channels) const {
    validateTextureId(textureId);
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    
    // Determine channels from internal format
    GLint internalFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    
    switch (internalFormat) {
        case GL_R8:
        case GL_R32F:
            channels = 1;
            break;
        case GL_RGB8:
        case GL_RGB32F:
            channels = 3;
            break;
        case GL_RGBA8:
        case GL_RGBA32F:
            channels = 4;
            break;
        default:
            channels = 0;
            break;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool GLRenderer::createShaderProgram() {
    // Compile vertex shader
    uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, getVertexShaderSource());
    if (vertexShader == 0) {
        return false;
    }
    
    // Compile fragment shader
    uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, getFragmentShaderSource());
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Create program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    
    // Link program
    if (!linkShaderProgram(mShaderProgram)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0;
        return false;
    }
    
    // Get uniform locations
    mMinValueUniform = glGetUniformLocation(mShaderProgram, "u_minValue");
    mMaxValueUniform = glGetUniformLocation(mShaderProgram, "u_maxValue");
    mTextureUniform = glGetUniformLocation(mShaderProgram, "u_texture");
    mChannelsUniform = glGetUniformLocation(mShaderProgram, "u_channels");
    
    // Get attribute locations
    mPositionAttribute = glGetAttribLocation(mShaderProgram, "a_position");
    mTexCoordAttribute = glGetAttribLocation(mShaderProgram, "a_texCoord");
    
    // Clean up shaders (they're linked into the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return (mMinValueUniform >= 0 && mMaxValueUniform >= 0 && mTextureUniform >= 0 && 
            mChannelsUniform >= 0 && mPositionAttribute >= 0 && mTexCoordAttribute >= 0);
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
    
    // Check compilation status
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        std::string error = getShaderError(shader);
        std::cerr << "Shader compilation failed: " << error << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool GLRenderer::linkShaderProgram(uint32_t program) {
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        std::string error = getProgramError(program);
        std::cerr << "Shader program linking failed: " << error << std::endl;
        return false;
    }
    
    return true;
}

std::string GLRenderer::getShaderError(uint32_t shader) const {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    
    if (length > 0) {
        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, log.data());
        return std::string(log.data());
    }
    
    return "Unknown shader error";
}

std::string GLRenderer::getProgramError(uint32_t program) const {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    
    if (length > 0) {
        std::vector<char> log(length);
        glGetProgramInfoLog(program, length, &length, log.data());
        return std::string(log.data());
    }
    
    return "Unknown program error";
}

bool GLRenderer::createVertexBuffers() {
    // Quad vertices (position + texture coordinates)
    float vertices[] = {
        // Position    // Texture Coords
        -1.0f, -1.0f,   0.0f, 0.0f,  // Bottom left
         1.0f, -1.0f,   1.0f, 0.0f,  // Bottom right
         1.0f,  1.0f,   1.0f, 1.0f,  // Top right
        -1.0f,  1.0f,   0.0f, 1.0f   // Top left
    };
    
    // Indices for quad (two triangles)
    uint32_t indices[] = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };
    
    // Generate buffers
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mEBO);
    
    // Bind VAO
    glBindVertexArray(mVAO);
    
    // Setup VBO
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Setup EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Setup vertex attributes
    // Position attribute
    glVertexAttribPointer(mPositionAttribute, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(mPositionAttribute);
    
    // Texture coordinate attribute
    glVertexAttribPointer(mTexCoordAttribute, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(mTexCoordAttribute);
    
    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return (mVAO != 0 && mVBO != 0 && mEBO != 0);
}

void GLRenderer::destroyVertexBuffers() {
    if (mVAO != 0) {
        glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
    }
    if (mVBO != 0) {
        glDeleteBuffers(1, &mVBO);
        mVBO = 0;
    }
    if (mEBO != 0) {
        glDeleteBuffers(1, &mEBO);
        mEBO = 0;
    }
}

uint32_t GLRenderer::getOpenGLInternalFormat(thor::data::ImageDataType pixelType, uint32_t channels) const {
    if (pixelType == thor::data::ImageDataType::UINT8) {
        switch (channels) {
            case 1: return GL_R8;
            case 3: return GL_RGB8;
            case 4: return GL_RGBA8;
            default: throw thor::core::OpenGLError("Unsupported channel count for UINT8: " + std::to_string(channels));
        }
    } else {
        switch (channels) {
            case 1: return GL_R32F;
            case 3: return GL_RGB32F;
            case 4: return GL_RGBA32F;
            default: throw thor::core::OpenGLError("Unsupported channel count for FLOAT32: " + std::to_string(channels));
        }
    }
}

uint32_t GLRenderer::getOpenGLFormat(uint32_t channels) const {
    switch (channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: throw thor::core::OpenGLError("Unsupported channel count: " + std::to_string(channels));
    }
}

uint32_t GLRenderer::getOpenGLType(thor::data::ImageDataType pixelType) const {
    switch (pixelType) {
        case thor::data::ImageDataType::UINT8: return GL_UNSIGNED_BYTE;
        case thor::data::ImageDataType::FLOAT32: return GL_FLOAT;
        default: throw thor::core::OpenGLError("Unsupported pixel type");
    }
}

void GLRenderer::bindTextureParameters() {
    // Set texture parameters for image display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

const char* GLRenderer::getVertexShaderSource() {
    return R"(
#version 330 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
    // Flip Y coordinate to correct upside-down display
    v_texCoord = vec2(a_texCoord.x, 1.0 - a_texCoord.y);
}
)";
}

const char* GLRenderer::getFragmentShaderSource() {
    return R"(
#version 330 core
in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D u_texture;
uniform float u_minValue;
uniform float u_maxValue;
uniform int u_channels;

void main() {
    vec4 texColor = texture(u_texture, v_texCoord);
    
    // Handle greyscale images: replicate red channel for single-channel textures
    vec3 finalColor = texColor.rgb;
    if (u_channels == 1) {
        finalColor = vec3(texColor.r, texColor.r, texColor.r);
    }

    // Apply min/max range mapping: (input - min) / (max - min)
    vec3 processedColor = (finalColor - u_minValue) / (u_maxValue - u_minValue);

    fragColor = vec4(processedColor, 1.0);
}
)";
}

void GLRenderer::validateInitialized() const {
    if (!mInitialized) {
        throw thor::core::OpenGLError("GLRenderer not initialized");
    }
}

void GLRenderer::validateTextureId(TextureID textureId) const {
    if (textureId == INVALID_TEXTURE_ID || !isValidTexture(textureId)) {
        throw thor::core::OpenGLError("Invalid texture ID: " + std::to_string(textureId));
    }
}

void GLRenderer::releaseResources() {
    destroyVertexBuffers();
    destroyShaderProgram();
}

} // namespace thor::rendering 