//
// Created by Shreejit Murthy on 28/10/2025
//

#include "graphics.hh"
#include "log.h"

void Asura::init(int debug_scale) {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    sdtx_desc_t sdtx_desc = {};
    sdtx_desc.fonts[FONT_KC854] = sdtx_font_kc854();
    sdtx_desc.fonts[FONT_ORIC] = sdtx_font_oric();
    sdtx_desc.logger.func = slog_func;
    sdtx_setup(&sdtx_desc);

    auto d = static_cast<float>(debug_scale);

    const float cx = sapp_widthf() / sapp_dpi_scale() * 1/d;
    const float cy = sapp_heightf() / sapp_dpi_scale() * 1/d;

    sdtx_canvas(cx, cy);
    sdtx_origin(1.f, 1.f);
}

void Asura::begin(const sg_pass_action& pass_action) {
    sg_pass pass = {};
    pass.action = pass_action;
    pass.swapchain = sglue_swapchain();

    sg_begin_pass(&pass);
}

void Asura::begin(sg_pass pass) {
    sg_begin_pass(&pass);
}

void Asura::end() {
    sdtx_draw();
    sg_end_pass();
    sg_commit();
}