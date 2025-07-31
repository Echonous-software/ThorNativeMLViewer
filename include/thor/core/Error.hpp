#pragma once

#include <stdexcept>
#include <string>

namespace thor::core {

// Base exception class for all Thor-specific errors
class ThorException : public std::exception {
public:
    explicit ThorException(const std::string& message) : mMessage(message) {}
    explicit ThorException(const char* message) : mMessage(message) {}
    
    const char* what() const noexcept override {
        return mMessage.c_str();
    }
    
protected:
    std::string mMessage;
};

// OpenGL-related errors
class OpenGLError : public ThorException {
public:
    explicit OpenGLError(const std::string& message) 
        : ThorException("OpenGL Error: " + message) {}
};

// Application initialization errors
class InitializationError : public ThorException {
public:
    explicit InitializationError(const std::string& message)
        : ThorException("Initialization Error: " + message) {}
};

// Future ML-related errors (mentioned in architecture)
class ModelLoadError : public ThorException {
public:
    explicit ModelLoadError(const std::string& message)
        : ThorException("Model Load Error: " + message) {}
};

class InferenceError : public ThorException {
public:
    explicit InferenceError(const std::string& message)
        : ThorException("Inference Error: " + message) {}
};

class DataFormatError : public ThorException {
public:
    explicit DataFormatError(const std::string& message)
        : ThorException("Data Format Error: " + message) {}
};

} // namespace thor::core 