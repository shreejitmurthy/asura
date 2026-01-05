//
// Created by Shreejit Murthy on 5/1/2026
//

#pragma once

/* General Math Functions */

namespace Asura::Math {

static constexpr double pi = 3.14159265358979323846264338327950288;

inline double radians(double deg) {
    return deg * pi / 180.f;
}

inline double degrees(double rad) {
    return rad * 180.f / pi;
}

}