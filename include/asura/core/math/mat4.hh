//
// Created by Shreejit Murthy on 18/12/2025
//

#pragma once

#include <cmath>
#include <algorithm>

#include "vec3.hh"

namespace Asura::Math {
struct Mat4 {
    float m[16];  // column-major (OpenGL style)

    constexpr Mat4() : m{0} {}
    constexpr explicit Mat4(float diag) : m{0} {
        m[0] = m[5] = m[10] = m[15] = diag;
    }

    // prefer Math::Mat4(1.f)
    static constexpr Mat4 identity() { return Mat4(1.f); }

    constexpr float& operator()(int row, int col) { return m[col * 4 + row]; }
    constexpr float  operator()(int row, int col) const { return m[col * 4 + row]; }

    constexpr Mat4 operator+(const Mat4& r) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] + r.m[i];
        return res;
    }
    constexpr Mat4 operator-(const Mat4& r) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] - r.m[i];
        return res;
    }
    constexpr Mat4 operator*(float s) const {
        Mat4 res;
        for (int i = 0; i < 16; i++) res.m[i] = m[i] * s;
        return res;
    }

    constexpr Mat4 operator*(const Mat4& r) const {
        Mat4 res;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                float sum = 0.f;
                for (int k = 0; k < 4; k++)
                    sum += (*this)(row, k) * r(k, col);
                res(row, col) = sum;
            }
        }
        return res;
    }

    constexpr Mat4 operator*=(const Mat4 r) {
        *this = *this * r;
        return *this;
    }

    constexpr Vec3 multiplyPoint(const Vec3& v) const {
        float x = v.x * (*this)(0, 0) + v.y * (*this)(0, 1) + v.z * (*this)(0, 2) + (*this)(0, 3);
        float y = v.x * (*this)(1, 0) + v.y * (*this)(1, 1) + v.z * (*this)(1, 2) + (*this)(1, 3);
        float z = v.x * (*this)(2, 0) + v.y * (*this)(2, 1) + v.z * (*this)(2, 2) + (*this)(2, 3);
        float w = v.x * (*this)(3, 0) + v.y * (*this)(3, 1) + v.z * (*this)(3, 2) + (*this)(3, 3);
        if (std::fabs(w) > 1e-6f) {
            float invW = 1.f / w;
            return {x * invW, y * invW, z * invW};
        }
        return {x, y, z};
    }

    constexpr Vec3 multiplyVector(const Vec3& v) const {
        // no translation
        return {
            v.x * (*this)(0, 0) + v.y * (*this)(0, 1) + v.z * (*this)(0, 2),
            v.x * (*this)(1, 0) + v.y * (*this)(1, 1) + v.z * (*this)(1, 2),
            v.x * (*this)(2, 0) + v.y * (*this)(2, 1) + v.z * (*this)(2, 2)
        };
    }

    // factory functions
    static constexpr Mat4 translate(const Vec3& t) {
        Mat4 m = identity();
        m(0, 3) = t.x;
        m(1, 3) = t.y;
        m(2, 3) = t.z;
        return m;
    }

    static constexpr Mat4 scale(const Vec3& s) {
        Mat4 m{};
        m(0, 0) = s.x; m(1, 1) = s.y; m(2, 2) = s.z; m(3, 3) = 1.f;
        return m;
    }

    static Mat4 rotateX(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(1, 1) = c; m(1,2) = -s;
        m(2, 1) = s; m(2,2) = c;
        return m;
    }

    static Mat4 rotateY(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(0, 0) = c; m(0,2) = s;
        m(2, 0) = -s; m(2,2) = c;
        return m;
    }

    static Mat4 rotateZ(float radians) {
        Mat4 m = identity();
        float c = std::cos(radians), s = std::sin(radians);
        m(0, 0) = c; m(0, 1) = -s;
        m(1, 0) = s; m(1, 1) = c;
        return m;
    }

    static Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar, bool depthZeroToOne = false) {
        Mat4 m{};
        const float rl = right - left;
        const float tb = top - bottom;
        const float fn = zFar - zNear;

        m(0, 0) = 2.f / rl;
        m(1, 1) = 2.f / tb;

        if (!depthZeroToOne) {
            // OpenGL style NDC: z in [-1, 1]
            m(2, 2) = -2.f / fn;
            m(0, 3) = -(right + left) / rl;
            m(1, 3) = -(top + bottom) / tb;
            m(2, 3) = -(zFar + zNear) / fn;
            m(3, 3) = 1.f;
        } else {
            // Vulkan/DirectX style NDC: z in [0, 1]
            m(2, 2) = 1.f / fn;
            m(0, 3) = -(right + left) / rl;
            m(1, 3) = -(top + bottom) / tb;
            m(2, 3) = -zNear / fn;
            m(3, 3) = 1.f;
        }
        return m;
    }

    static Mat4 orthoCentered(float width, float height, float zNear, float zFar, bool depthZeroToOne = false) {
        float hw = width  * 0.5f;
        float hh = height * 0.5f;
        return ortho(-hw, hw, -hh, hh, zNear, zFar, depthZeroToOne);
    }
};
} // Asura