#include <gtest/gtest.h>
#include <thor/core/Error.hpp>

namespace thor::core::test {

TEST(ThorExceptionTest, BasicConstruction) {
    const std::string message = "Test error message";
    ThorException ex(message);
    
    EXPECT_STREQ(ex.what(), message.c_str());
}

TEST(ThorExceptionTest, CStringConstruction) {
    const char* message = "Test error message";
    ThorException ex(message);
    
    EXPECT_STREQ(ex.what(), message);
}

TEST(OpenGLErrorTest, ErrorMessageFormatting) {
    const std::string message = "OpenGL context creation failed";
    OpenGLError ex(message);
    
    std::string expected = "OpenGL Error: " + message;
    EXPECT_STREQ(ex.what(), expected.c_str());
}

TEST(InitializationErrorTest, ErrorMessageFormatting) {
    const std::string message = "Failed to initialize subsystem";
    InitializationError ex(message);
    
    std::string expected = "Initialization Error: " + message;
    EXPECT_STREQ(ex.what(), expected.c_str());
}

TEST(ModelLoadErrorTest, ErrorMessageFormatting) {
    const std::string message = "Could not load model file";
    ModelLoadError ex(message);
    
    std::string expected = "Model Load Error: " + message;
    EXPECT_STREQ(ex.what(), expected.c_str());
}

TEST(InferenceErrorTest, ErrorMessageFormatting) {
    const std::string message = "Inference failed";
    InferenceError ex(message);
    
    std::string expected = "Inference Error: " + message;
    EXPECT_STREQ(ex.what(), expected.c_str());
}

TEST(DataFormatErrorTest, ErrorMessageFormatting) {
    const std::string message = "Invalid data format";
    DataFormatError ex(message);
    
    std::string expected = "Data Format Error: " + message;
    EXPECT_STREQ(ex.what(), expected.c_str());
}

TEST(ExceptionHierarchyTest, PolymorphicBehavior) {
    std::unique_ptr<std::exception> ex = std::make_unique<OpenGLError>("test");
    
    // Should be able to catch as std::exception
    try {
        throw OpenGLError("test error");
    } catch (const std::exception& e) {
        EXPECT_TRUE(std::string(e.what()).find("OpenGL Error:") != std::string::npos);
    }
    
    // Should be able to catch as ThorException
    try {
        throw InitializationError("init error");
    } catch (const ThorException& e) {
        EXPECT_TRUE(std::string(e.what()).find("Initialization Error:") != std::string::npos);
    }
}

} // namespace thor::core::test 