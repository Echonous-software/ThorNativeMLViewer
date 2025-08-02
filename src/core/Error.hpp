#pragma once

#include <stdexcept>
#include <fmt/format.h>

namespace echonous {

class Error : public std::runtime_error {
public:
    explicit Error(const char* message) : std::runtime_error(message) {}
    explicit Error(const std::string& message) : std::runtime_error(message) {}
};

}