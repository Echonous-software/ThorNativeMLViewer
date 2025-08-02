#pragma once

#include <core/Error.hpp>

namespace echonous {

class DataError : public Error {
public:
    DataError(const char* message) : Error(message) {}
    DataError(const std::string& message) : Error(message) {}
};

class DataReadError : public DataError {
public:
    DataReadError(const char* message) : DataError(message) {}
    DataReadError(const std::string& message) : DataError(message) {}
};

} // namespace echonous