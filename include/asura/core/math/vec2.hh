//
// Created by Shreejit Murthy on 18/12/2025
//

#pragma once

#include <cmath>
#include <algorithm>

namespace Asura::Math {

struct Vec2 {
    float x{0}, y{0};

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    static constexpr Vec2 zero() { return Vec2(0, 0); }

    // basic arithmetic
    constexpr Vec2 operator+(const Vec2& r) const noexcept { return {x + r.x, y + r.y}; }
    constexpr Vec2 operator-(const Vec2& r) const noexcept { return {x - r.x, y - r.y}; }
    constexpr Vec2 operator-() const noexcept { return {-x, -y}; }

    constexpr bool operator==(const Vec2& r) const noexcept { return x == r.x && y == r.y; }
    constexpr Vec2& operator+=(const Vec2& r) noexcept { x += r.x; y += r.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& r) noexcept { x -= r.x; y -= r.y; return *this; }

    // scalar ops
    constexpr Vec2 operator*(float s) const noexcept { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const noexcept { return {x / s, y / s}; }
    constexpr Vec2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr Vec2& operator/=(float s) noexcept { x /= s; y /= s; return *this; }

    friend constexpr Vec2 operator*(float s, const Vec2& v) noexcept { return {v.x * s, v.y * s}; }

    // helpers
    constexpr float dot(const Vec2& r) const noexcept { return x * r.x + y * r.y; }
    float length() const noexcept { return std::sqrt(dot(*this)); }
    constexpr float length2() const noexcept { return dot(*this); }

    Vec2 normalized(float eps = 1e-6f) const noexcept {
        float len2 = length2();
        if (len2 <= eps * eps) return {0.f, 0.f};
        float inv = 1.f / std::sqrt(len2);
        return {x * inv, y * inv};
    }

    // 90 degree CCW perpendicular 
    constexpr Vec2 perp() const noexcept { return {-y, x}; }

    static constexpr Vec2 lerp(const Vec2& a, const Vec2& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    }
};

struct iVec2 {
    int x{0}, y{0};

    constexpr iVec2() = default;
    constexpr iVec2(int x_, int y_) : x(x_), y(y_) {}

    // equality
    constexpr bool operator==(const iVec2& r) const noexcept { return x == r.x && y == r.y; }

    // basic arithmetic
    constexpr iVec2 operator+(const iVec2& r) const noexcept { return {x + r.x, y + r.y}; }
    constexpr iVec2 operator-(const iVec2& r) const noexcept { return {x - r.x, y - r.y}; }
    constexpr iVec2 operator-() const noexcept { return {-x, -y}; }

    constexpr bool operator==(const iVec2& r) noexcept { return x == r.x && y == r.y; }
    constexpr iVec2& operator+=(const iVec2& r) noexcept { x += r.x; y += r.y; return *this; }
    constexpr iVec2& operator-=(const iVec2& r) noexcept { x -= r.x; y -= r.y; return *this; }

    // scalar ops (integer)
    constexpr iVec2 operator*(int s) const noexcept { return {x * s, y * s}; }
    constexpr iVec2& operator*=(int s) noexcept { x *= s; y *= s; return *this; }

    constexpr iVec2 operator/(int s) const noexcept { return {x / s, y / s}; }
    constexpr iVec2& operator/=(int s) noexcept { x /= s; y /= s; return *this; }
    friend constexpr iVec2 operator*(int s, const iVec2& v) noexcept { return {v.x * s, v.y * s}; }

    // helpers
    constexpr int dot(const iVec2& r) const noexcept { return x * r.x + y * r.y; }
    constexpr int length2() const noexcept { return dot(*this); }
    double length() const noexcept { return std::sqrt(static_cast<double>(length2())); }

    // 90 degree CCW perpendicular
    constexpr iVec2 perp() const noexcept { return {-y, x}; }

    // conversions to float vector for real-valued ops
    explicit constexpr operator Vec2() const noexcept { return Vec2{static_cast<float>(x), static_cast<float>(y)}; }

    // real-valued helpers that return Vec2
    Vec2 normalized(double eps = 1e-6) const noexcept {
        double len2 = static_cast<double>(length2());
        if (len2 <= eps * eps) return {0.f, 0.f};
        double inv = 1.0 / std::sqrt(len2);
        return {static_cast<float>(x * inv), static_cast<float>(y * inv)};
    }

    static Vec2 lerp(const iVec2& a, const iVec2& b, float t) noexcept {
        return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t};
    }
};

} // Asura::Math