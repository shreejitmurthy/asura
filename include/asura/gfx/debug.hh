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

static constexpr int FONT_KC854 = 0;
static constexpr int FONT_ORIC  = 1;

namespace Asura {

class Debug {
public:
    static void print(const std::string &text, sg_color color = sg_white, int font = FONT_KC854);
    static void temp(const std::string& text, float lifespan, float dt, sg_color color = sg_white, int font = FONT_KC854);
    /* Longer messages in vectors are better suited for Oric Atmos */
    static void print(const std::vector<std::string>& text, const std::string& label = "", sg_color color = sg_white, int font = FONT_ORIC);
    static void temp(const std::vector<std::string>& text, float lifespan, float dt, const std::string& label = "", sg_color color = sg_white, int font = FONT_ORIC);

    static void resize(Math::Vec2 dim);
};
} // Asura