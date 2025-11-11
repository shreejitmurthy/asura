//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <sokol/sokol_app.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include "debug.hh"
#include "sprite.hh"
#include "font.hh"

#define FONT_KC854 (0)
#define FONT_ORIC  (1)

namespace asura {

void init(int debug_scale = 2);
void begin(sg_pass_action& pass_action);
void begin(sg_pass pass);
void end();

} // namespace asura
