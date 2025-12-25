//
// Created by Shreejit Murthy on 18/12/2025
//

#pragma once

#include <cmath>
#include <algorithm>

namespace Asura::Math {
struct Vec3 {
    float x{0}, y{0}, z{0};

    constexpr Vec3() = default;
    constexpr Vec3(float x_, float y_, float z_ = 0) : x(x_), y(y_), z(z_) {}

    // basic arithmetic
    constexpr Vec3 operator+(const Vec3& r) const noexcept { return {x + r.x, y + r.y, z + r.z}; }
    constexpr Vec3 operator-(const Vec3& r) const noexcept { return {x - r.x, y - r.y, z - r.z}; }
    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vec3& operator+=(const Vec3& r) noexcept { x += r.x; y += r.y; z += r.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& r) noexcept { x -= r.x; y -= r.y; z -= r.z; return *this; }

    // scalar ops
    constexpr Vec3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator/(float s) const noexcept { return {x / s, y / s, z / s}; }
    constexpr Vec3& operator*=(float s) noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr Vec3& operator/=(float s) noexcept { x /= s; y /= s; z /= s; return *this; }

    friend constexpr Vec3 operator*(float s, const Vec3& v) noexcept { return {v.x * s, v.y * s, v.z * s}; }

    // helpers
    constexpr float dot(const Vec3& r) const noexcept { return x * r.x + y * r.y + z * r.z; }
    constexpr Vec3 cross(const Vec3& r) const noexcept {
        return { y * r.z - z * r.y,
                 z * r.x - x * r.z,
                 x * r.y - y * r.x };
    }

    float length() const noexcept { return std::sqrt(dot(*this)); }
    constexpr float length2() const noexcept { return dot(*this); }

    Vec3 normalized(float eps = 1e-6f) const noexcept {
        float len2 = length2();
        if (len2 <= eps * eps) return {0.f, 0.f, 0.f};
        float inv = 1.f / std::sqrt(len2);
        return {x * inv, y * inv, z * inv};
    }

    static constexpr Vec3 lerp(const Vec3& a, const Vec3& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
    }
};
} // Asura::Math