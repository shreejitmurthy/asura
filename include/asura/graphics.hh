//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include "debug.hh"
#include "device.hh"
#include "sprite.hh"
#include "font.hh"

#define FONT_KC854 (0)
#define FONT_ORIC  (1)

namespace Asura {
void init(const Device& device);
void begin(const sg_pass_action& pass_action);
void begin(sg_pass pass);
void end();
} // Asura
