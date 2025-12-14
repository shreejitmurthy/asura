//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <string>

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_debugtext.h>
#include <sokol/util/sokol_color.h>

#include "device.hh"

#define FONT_KC854 (0)
#define FONT_ORIC  (1)

namespace Asura::Debug {

inline void print(const std::string &text, sg_color colour = sg_white, int font = FONT_KC854) {
    if (!Device::instance().debug) return;
    sdtx_font(font);
    sdtx_color3b(colour.r * 255, colour.g * 255, colour.b * 255);
    sdtx_printf("%s\n\n", text.c_str());
}

} // Asura::Debug