//
// Created by Shreejit Murthy on 28/10/2025
//

#include "graphics.hh"
#include "core/log.h"

// TODO: Reconsider globals.
Asura::WindowBackend backend;
sg_swapchain sc;

void Asura::init(const Device& device) {
    sg_desc desc = {};

    auto win = device.win_backend;
    backend = win;

    std::string win_backend;
    switch (win) {
        case WindowBackend::SAPP:
            win_backend = "sokol_app";
            desc.environment = sglue_environment();
            break;
        case WindowBackend::GLFW:
            win_backend = "GLFW";
            // fall-through as the swapchain behaviour is same across GLFW and SDL
        case WindowBackend::SDL:
            win_backend = "SDL";
            sc = {};
            sc.width = device.width;
            sc.height = device.height;
            #if defined(__APPLE__)
                sc.color_format = SG_PIXELFORMAT_BGRA8;
            #else
                sc.color_format = SG_PIXELFORMAT_RGBA8;
            #endif
            sc.sample_count = 1;
            break;
    }

    // TODO:Eventually swap out slog_func for spdlog logger.
    desc.logger.func = slog_func;
    sg_setup(&desc);

    std::string gfx_backend;
    switch (sg_query_backend()) {
        case SG_BACKEND_METAL_MACOS:
        case SG_BACKEND_METAL_IOS:
        case SG_BACKEND_METAL_SIMULATOR:
            gfx_backend = "Metal";
            break;
        case SG_BACKEND_D3D11:
            gfx_backend = "D3D11";
            break;
        case SG_BACKEND_GLCORE:
            gfx_backend = "OpenGL";
            break;
    }

    log().info("Window Backend: {}", win_backend);
    log().info("Graphics Backend: {}", gfx_backend);

    if (device.debug) {
        log().info("Enabled gfx debugging at {}px", device.debug_scale);

        sdtx_desc_t sdtx_desc = {};
        sdtx_desc.fonts[FONT_KC854] = sdtx_font_kc854();
        sdtx_desc.fonts[FONT_ORIC] = sdtx_font_oric();
        sdtx_desc.logger.func = slog_func;
        sdtx_setup(&sdtx_desc);

        float d = static_cast<float>(device.debug_scale);

        int dpi_scale = device.high_dpi ? 2 : 1;

        const float cx = static_cast<float>(device.width)  / dpi_scale * 1 / d;
        const float cy = static_cast<float>(device.height) / dpi_scale * 1 / d;

        sdtx_canvas(cx, cy);
        sdtx_origin(1.f, 1.f);
    }
}

void Asura::begin(const sg_pass_action& pass_action) {
    sg_pass pass = {};
    pass.action = pass_action;
    if (backend == WindowBackend::SAPP) {
        pass.swapchain = sglue_swapchain();
    } else {
        pass.swapchain = sc;
    }

    sg_begin_pass(&pass);
}

void Asura::begin(sg_pass pass) {
    sg_begin_pass(&pass);
}

void Asura::end() {
    if (Device::instance().debug) sdtx_draw();
    sg_end_pass();
    sg_commit();
}