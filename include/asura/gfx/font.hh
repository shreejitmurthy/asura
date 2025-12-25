//
// Created by Shreejit Murthy on 10/11/2025.
//

#pragma once

#include "resource.hh"
#include "device.hh"

// FIXME: these includes are chopped
#include "../core/math.hh"
#include "../core/utils.h"

#include <string>
#include <vector>

#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_color.h>

#include "shaders/shader.glsl.h"

#include <stb_truetype.h>

static constexpr int FIRST_CHAR = 32;  // start with ASCII code 33 exclamation mark.
static constexpr int NUM_CHARS  = 95;  // end with ASCII code 127

namespace Asura {
struct Vertex {
    float x, y;
    float u, v;
    Math::Vec4 color;
};

struct Font {
    int id;
    std::string name;
    int size;

    int w, h;
    std::vector<std::uint8_t> bitmap;

    sg_image atlas;  // font texture
    sg_view view;
    std::array<stbtt_bakedchar, NUM_CHARS> chars;

    // per frame batch for this font
    struct {
        std::vector<Vertex> verts;
        std::vector<uint16_t> indices;
    } batch;
};

class FontRenderer {
public:
    void init(const std::string& fonts_dir, std::vector<ResourceDef> reg = {});
     template <typename E>
        requires std::is_enum_v<E>
    void queue(E id, std::string_view text, Math::Vec2 pos, float scale = 1.f, sg_color tint = sg_white) {
        _queue_text(std::to_underlying(id), text, pos, scale, tint);
    }

    void render(Math::Mat4 view = Math::Mat4(1.f));

    void resize(Math::Vec2 dim, Math::Vec2 virtual_dim) { Utils::Gfx::update_projection_matrix(dim, virtual_dim, vs_params.mvp); }

private:  
    void _clear();

    void _init_fonts(const char* dir);
    void _init_fr();

    void _queue_text(int id, std::string_view text, Math::Vec2 pos, float scale, sg_color tint);

    Font* _find_font(int id);

    // remove
    std::array<stbtt_bakedchar, NUM_CHARS> char_data = {};
    
    std::vector<ResourceDef> kFontDefs;
    std::vector<Font> fonts;

    std::array<int, 256> id_to_font_index = {};

    sg_buffer   vbuf = {};
    sg_buffer   ibuf = {};
    sg_pipeline pip  = {};
    sg_sampler  smp  = {};

    static constexpr int MAX_GLYPHS = 4096;

    text_params_t vs_params;
};

} // Asura
