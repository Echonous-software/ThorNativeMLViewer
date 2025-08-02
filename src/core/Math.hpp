#pragma once

#include <string>
#include <fmt/format.h>

namespace echonous {

template <typename T> struct TVec2 {
    T x;
    T y;

    constexpr TVec2() noexcept : x(), y() {}
    constexpr TVec2(T x, T y) noexcept : x(x), y(y) {}

    constexpr TVec2 operator+(const TVec2& other) const noexcept {
        return TVec2(x + other.x, y + other.y);
    }

    constexpr TVec2 operator-(const TVec2& other) const noexcept {
        return TVec2(x - other.x, y - other.y);
    }

    constexpr TVec2 operator*(T scalar) const noexcept {
        return TVec2(x * scalar, y * scalar);
    }

    constexpr TVec2 operator/(T scalar) const noexcept {
        return TVec2(x / scalar, y / scalar);
    }

    constexpr TVec2& operator+=(const TVec2& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr TVec2& operator-=(const TVec2& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr TVec2& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr TVec2& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr T sqlength() const noexcept {
        return x * x + y * y;
    }

    constexpr T length() const noexcept {
        return std::sqrt(sqlength());
    }
    constexpr T dot(const TVec2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    constexpr TVec2 normalized() const noexcept {
        return *this / length();
    }
};

template <typename T>
std::string format_as(const TVec2<T>& vec) {
    return fmt::format("({}, {})", vec.x, vec.y);
}

using Vec2 = TVec2<float>;
using IVec2 = TVec2<int>;

template <typename T>
struct TVec3 {
    T x;
    T y;
    T z;

    constexpr TVec3() noexcept : x(), y(), z() {}
    constexpr TVec3(T x, T y, T z) noexcept : x(x), y(y), z(z) {}

    constexpr TVec3 operator+(const TVec3& other) const noexcept {
        return TVec3(x + other.x, y + other.y, z + other.z);
    }

    constexpr TVec3 operator-(const TVec3& other) const noexcept {
        return TVec3(x - other.x, y - other.y, z - other.z);
    }

    constexpr TVec3 operator*(T scalar) const noexcept {
        return TVec3(x * scalar, y * scalar, z * scalar);
    }

    constexpr TVec3 operator/(T scalar) const noexcept {
        return TVec3(x / scalar, y / scalar, z / scalar);
    }

    constexpr TVec3& operator+=(const TVec3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr TVec3& operator-=(const TVec3& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr TVec3& operator*=(T scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr TVec3& operator/=(T scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    constexpr T sqlength() const noexcept {
        return x * x + y * y + z * z;
    }

    constexpr T length() const noexcept {
        return std::sqrt(sqlength());
    }
    constexpr T dot(const TVec3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr TVec3 normalized() const noexcept {
        return *this / length();
    }

    constexpr TVec3 cross(const TVec3& other) const noexcept {
        return TVec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }
};

template <typename T>
std::string format_as(const TVec3<T>& vec) {
    return fmt::format("({}, {}, {})", vec.x, vec.y, vec.z);
}

using Vec3 = TVec3<float>;
using IVec3 = TVec3<int>;
} // namespace echonous