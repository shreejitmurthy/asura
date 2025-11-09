//
// Created by Shreejit Murthy on 28/10/2025
//

#include "graphics.hh"

void asura::init(int debug_scale) {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    sdtx_desc_t sdtx_desc = {};
    sdtx_desc.fonts[FONT_KC854] = sdtx_font_kc854();
    sdtx_desc.fonts[FONT_ORIC] = sdtx_font_oric();
    sdtx_desc.logger.func = slog_func;
    sdtx_setup(&sdtx_desc);

    sdtx_canvas((sapp_widthf() / sapp_dpi_scale()) * 1/debug_scale, (sapp_heightf() / sapp_dpi_scale()) * 1/debug_scale);
    sdtx_origin(1.f, 1.f);
}

void asura::begin(sg_pass_action& pass_action) {
    sg_pass pass = {};
    pass.action = pass_action;
    pass.swapchain = sglue_swapchain();

    sg_begin_pass(&pass);
}

void asura::begin(sg_pass pass) {
    sg_begin_pass(&pass);
}

void asura::end() {
    sdtx_draw();
    sg_end_pass();
    sg_commit();
}