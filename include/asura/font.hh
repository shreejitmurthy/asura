//
// Created by Shreejit Murthy on 10/11/2025.
//

#pragma once

#include "resource.hh"

#include <string>
#include <vector>
#include <memory>

#include <sokol/sokol_gfx.h>
#include <sokol/sokol_app.h>
#include <sokol/util/sokol_color.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_truetype.h"

inline static glm::mat4 get_default_projectionf() {
    return glm::ortho(0.f, sapp_widthf() / sapp_dpi_scale(), sapp_heightf() / sapp_dpi_scale(), 0.f, -1.f, 1.f);
}

static constexpr int FIRST_CHAR = 32;  // start with ASCII code 33 exclamation mark.
static constexpr int NUM_CHARS  = 96;

namespace Asura {
struct Font {
    int id;
    std::string name;
    int size;

    sg_image atlas;
    std::array<stbtt_bakedchar, NUM_CHARS> char_data;
};

class FontRenderer {
public:
    void init(const std::string& fonts_dir, std::vector<ResourceDef> reg = {});
     template <typename E>
        requires std::is_enum_v<E>
    void queue(E id, std::string_view text, glm::vec2 pos, float scale = 1.f, sg_color tint = sg_white) {
        _queue_text(std::to_underlying(id), text, pos, scale, tint);
    }

    void render(glm::mat4 projection = get_default_projectionf(), glm::mat4 view = glm::mat4(1.f));

private:
    const int font_bitmap_w = 256;
    const int font_bitmap_h = 256;
    
    void _clear();
    void _init_fonts(const char* dir);
    void _init_fr();

    void _queue_text(int id, std::string_view text, glm::vec2 pos, float scale, sg_color tint);

    // batching stuff
    struct Vertex {
        float x, y;
        float u, v;
        glm::vec4 color;
    };

    struct Font {
        int id;
        std::string name;
        int size;

        glm::vec2 bitmap_size;

        sg_image atlas;  // font texture
        sg_view view;
        std::array<stbtt_bakedchar, NUM_CHARS> chars;

        // per frame batch for this font
        struct {
            std::vector<Vertex> verts;
            std::vector<uint16_t> indices;
        } batch;
    };

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
};

} // Asura
