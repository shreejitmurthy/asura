//
// Created by Shreejit Murthy on 10/11/2025.
//

#include <utility>

#include "font.hh"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "stb_image_write.h"

#include "utils.h"
#include "log.h"

#include "shaders/shader.glsl.h"

#define offsetfr(v) (int)offsetof(Vertex, v)

void Asura::FontRenderer::init(const std::string &fonts_dir, std::vector<ResourceDef> reg) {
    id_to_font_index.fill(-1);
    kFontDefs = std::move(reg);
    _init_fonts(fonts_dir.c_str());
    _init_fr();
}

void Asura::FontRenderer::render(glm::mat4 projection, glm::mat4 view) {
    sg_apply_pipeline(pip);

    glm::mat4 mvp = projection * view;

    text_params_t vs_params;
    vs_params.mvp = mvp;

    for (auto& f : fonts) {
        auto& verts = f.batch.verts;
        auto& indices = f.batch.indices;

        if (verts.empty() || indices.empty()) continue;

        sg_update_buffer(vbuf, SG_RANGE(verts));
        // sg_update_buffer(ibuf, SG_RANGE(indices));

        sg_bindings bind = {};
        bind.vertex_buffers[0] = vbuf;
        bind.index_buffer = ibuf;
        bind.views[VIEW_font_tex] = f.view;
        bind.samplers[SMP_font_smp] = smp;

        sg_apply_bindings(&bind);
        sg_apply_uniforms(UB_text_params, SG_RANGE(vs_params));
        sg_draw(0, static_cast<int>(indices.size()), 1);
    }

    _clear();  /* potential issues in the future */
}

void Asura::FontRenderer::_clear() {
    for (auto& f : fonts) {
        f.batch.verts.clear();
        f.batch.indices.clear();
    }
}

void Asura::FontRenderer::_init_fonts(const char *dir) {
    const int font_bitmap_w = 256;
    const int font_bitmap_h = 256;

    fonts.clear();
    fonts.reserve(kFontDefs.size());

    for (auto& def : kFontDefs) {
        Font font = {};
        font.id   = def.id;
        font.name = def.name;
        font.size = def.size;

        auto png = join_path_png(dir, def.name);
        auto ttf = join_path_ttf(dir, def.name);

        std::vector<unsigned char> bitmap(font_bitmap_w * font_bitmap_h);

        std::size_t ttf_size;
        auto ttf_data = read_file(ttf.c_str(), ttf_size);

        int ret = stbtt_BakeFontBitmap(
            ttf_data,
            0,
            static_cast<float>(def.size) - 1,
            bitmap.data(),
            font_bitmap_w,
            font_bitmap_h,
            FIRST_CHAR,
            NUM_CHARS,
            font.chars.data()
        );
        if (ret == 0) die(std::format("Failed to load font at: {}", ttf));

        if (!std::filesystem::exists(png)) {
            stbi_write_png(png.c_str(), font_bitmap_w, font_bitmap_h, 1, bitmap.data(), font_bitmap_w);
            log().info("Generated bitmap font: {}", png);
        } else {
            log().info("Reused bitmap font: {}", png);
        }

        sg_image_desc img = {};
        img.width  = font_bitmap_w;
        img.height = font_bitmap_h;
        img.pixel_format = SG_PIXELFORMAT_R8;
        img.usage.immutable = true;
        img.data.mip_levels[0].ptr  = bitmap.data();
        img.data.mip_levels[0].size = bitmap.size();
        font.atlas = sg_make_image(&img);

        sg_view_desc vd = {};
        vd.texture.image = font.atlas;
        font.view = sg_make_view(&vd);

        int idx = (int)fonts.size();
        fonts.push_back(std::move(font));

        if (def.id >= 0 && def.id < (int)id_to_font_index.size()) {
            id_to_font_index[def.id] = idx;
        }
    }
}

void Asura::FontRenderer::_init_fr() {
    sg_buffer_desc vb = {};
    vb.size = MAX_GLYPHS * 4 * sizeof(Vertex);
    vb.usage.stream_update = true;
    vb.usage.vertex_buffer = true;
    vbuf = sg_make_buffer(&vb);

    std::vector<uint16_t> tmp;
    tmp.reserve(MAX_GLYPHS * 6);
    for (int i = 0; i < MAX_GLYPHS; ++i) {
        uint16_t base = static_cast<uint16_t>(i * 4);
        tmp.push_back(base + 0);
        tmp.push_back(base + 1);
        tmp.push_back(base + 2);
        tmp.push_back(base + 0);
        tmp.push_back(base + 2);
        tmp.push_back(base + 3);
    }

    sg_buffer_desc ib = {};
    ib.usage.index_buffer = true;
    ib.data = SG_RANGE(tmp);
    ibuf = sg_make_buffer(&ib);

    sg_sampler_desc sd = {};
    sd.min_filter = SG_FILTER_LINEAR;
    sd.mag_filter = SG_FILTER_NEAREST;
    smp = sg_make_sampler(&sd);

    sg_shader shd = sg_make_shader(text_shader_desc(sg_query_backend()));

    sg_pipeline_desc pip_desc = {};
    pip_desc.layout.buffers[0].stride = sizeof(Vertex);
    
    pip_desc.layout.attrs[ATTR_text_position].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_text_position].offset = offsetfr(x);

    pip_desc.layout.attrs[ATTR_text_texcoord0].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.layout.attrs[ATTR_text_texcoord0].offset = offsetfr(u);

    pip_desc.layout.attrs[ATTR_text_color0].format = SG_VERTEXFORMAT_FLOAT4;
    pip_desc.layout.attrs[ATTR_text_color0].offset = offsetfr(color);

    pip_desc.colors[0].blend.enabled          = true;
    pip_desc.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_rgb           = SG_BLENDOP_ADD;
    pip_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_alpha         = SG_BLENDOP_ADD;
    
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pip_desc.shader = shd;

    pip = sg_make_pipeline(&pip_desc);
}

void Asura::FontRenderer::_queue_text(int id, std::string_view text, glm::vec2 pos, sg_color tint, float scale) {
    Font* font = _find_font(id);
    if (!font || text.empty()) return;
    
    auto& verts   = font->batch.verts;
    auto& indices = font->batch.indices;

    glm::vec4 col = {tint.r, tint.g, tint.b, tint.a};

    float x = pos.x;
    float y = pos.y;

    for (char c : text) {
        if (c == '\n') {
            x = pos.x;
            y += font->size * scale;
            continue;
        }

        int ci = (int)(unsigned char)c - FIRST_CHAR;
        if (ci < 0 || ci >= NUM_CHARS) continue;

        stbtt_aligned_quad q;
        // TODO: clarify magic numbers
        stbtt_GetBakedQuad(font->chars.data(), 256, 256, ci, &x, &y, &q, true);

        float x0 = pos.x + (q.x0 - pos.x) * scale;
        float y0 = pos.y + (q.y0 - pos.y) * scale;
        float x1 = pos.x + (q.x1 - pos.x) * scale;
        float y1 = pos.y + (q.y1 - pos.y) * scale;

        std::uint16_t base = static_cast<std::uint16_t>(verts.size());

        verts.push_back(Vertex { x0, y0, q.s0, q.t0, col });
        verts.push_back(Vertex { x1, y0, q.s1, q.t0, col });
        verts.push_back(Vertex { x1, y1, q.s1, q.t1, col });
        verts.push_back(Vertex { x0, y1, q.s0, q.t1, col });

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
}

Asura::FontRenderer::Font* Asura::FontRenderer::_find_font(int id) {
    if (id < 0 || id >= (int)id_to_font_index.size()) return nullptr;
    int idx = id_to_font_index[id];
    if (idx < 0 || idx >= (int)fonts.size()) return nullptr;
    return &fonts[idx];
}
