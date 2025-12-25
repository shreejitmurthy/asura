//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <string>

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_debugtext.h>
#include <sokol/util/sokol_color.h>

#include "device.hh"
#include "../core/math.hh"

#define FONT_KC854 (0)
#define FONT_ORIC  (1)

namespace Asura::Debug {

inline void print(const std::string &text, sg_color colour = sg_white, int font = FONT_KC854) {
    if (!Device::instance().debug) return;
    sdtx_font(font);
    sdtx_color3b(colour.r * 255, colour.g * 255, colour.b * 255);
    sdtx_printf("%s\n\n", text.c_str());
}

inline void resize(Math::Vec2 dim) {
    auto device = Device::instance();
    if (!device.debug) return;
    float d = static_cast<float>(device.debug_scale);
    int dpi_scale = device.high_dpi ? 2 : 1;
    const float cx = dim.x  / dpi_scale * 1 / d;
    const float cy = dim.y  / dpi_scale * 1 / d;
    sdtx_canvas(cx, cy);
    sdtx_origin(1.f, 1.f);
}

} // Asura::Debug