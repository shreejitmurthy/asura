//
// Created by Shreejit Murthy on 18/12/2025
//

#pragma once

namespace Asura::Math {
struct Vec4 {
    float x{0}, y{0}, z{0}, w{0};

    constexpr Vec4() = default;
    constexpr Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    // basic arithmetic
    constexpr Vec4 operator+(const Vec4& r) const noexcept { return {x + r.x, y + r.y, z + r.z, w + r.w}; }
    constexpr Vec4 operator-(const Vec4& r) const noexcept { return {x - r.x, y - r.y, z - r.z, w - r.w}; }
    constexpr Vec4 operator-() const noexcept { return {-x, -y, -z, -w}; }

    constexpr Vec4& operator+=(const Vec4& r) noexcept { x += r.x; y += r.y; z += r.z; w += r.w; return *this; }
    constexpr Vec4& operator-=(const Vec4& r) noexcept { x -= r.x; y -= r.y; z -= r.z; w -= r.w; return *this; }

    // scalar ops
    constexpr Vec4 operator*(float s) const noexcept { return {x * s, y * s, z * s, w * s}; }
    constexpr Vec4 operator/(float s) const noexcept { return {x / s, y / s, z / s, w / s}; }
    constexpr Vec4& operator*=(float s) noexcept { x *= s; y *= s; z *= s; w *= s; return *this; }
    constexpr Vec4& operator/=(float s) noexcept { x /= s; y /= s; z /= s; w /= s; return *this; }

    friend constexpr Vec4 operator*(float s, const Vec4& v) noexcept { return {v.x * s, v.y * s, v.z * s, v.w * s}; }

    static constexpr Vec4 lerp(const Vec4& a, const Vec4& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
    }
};
} // Asura::Math