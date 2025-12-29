//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <string>
#include <vector>

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_debugtext.h>
#include <sokol/util/sokol_color.h>

#include "device.hh"
#include "../core/math.hh"
#include "../core/log.h"  // temp

#define FONT_KC854 (0)
#define FONT_ORIC  (1)

namespace Asura {

class Debug {
public:
    static void print(const std::string &text, sg_color color = sg_white, int font = FONT_KC854);
    static void print(std::vector<std::string>& text, sg_color color = sg_white, int font = FONT_KC854);

    static void temp(const std::string& text, float lifespan, float dt, sg_color color = sg_white, int font = FONT_KC854);
    static void temp(std::vector<std::string>& text, float lifespan, float dt, sg_color color = sg_white, int font = FONT_KC854);

    static void resize(Math::Vec2 dim);
};
} // Asura