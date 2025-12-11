//
// Created by Shreejit Murthy on 10/11/2025.
//

#include <utility>

#include "font.hh"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <stb_image_write.h>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "utils.h"
#include "log.h"

#include "shaders/shader.glsl.h"

#define offsetfr(v) (int)offsetof(Vertex, v)

#define SG_VECTOR_RANGE(v) sg_range{ (v).data(), (v).size() * sizeof((v)[0]) }

std::tuple<float, float> estimateGlyphCellSize(unsigned char* ttf, float pixel_h, int first_cdepnt, int num_cdepnt) {
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf, stbtt_GetFontOffsetForIndex(ttf, 0));

    float scale = stbtt_ScaleForPixelHeight(&font, pixel_h);

    float max_w = 0;
    float max_h = 0;

    for (int i = 0; i < num_cdepnt; ++i) {
        int point = first_cdepnt + i;

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font, point, scale, scale, &x0, &y0, &x1, &y1);

        float w = static_cast<float>(x1 - x0);
        float h = static_cast<float>(y1 - y0);

        if (w > max_w) max_w = w;
        if (h > max_h) max_h = h;
    }

    float padding = 0.f;
    return {max_w + 2.f + padding, max_h + 2.f * padding};
}

std::tuple<int, int> estimateAtlasSize(float gw, float gh, int N) {
    const double cols_f = std::sqrt(static_cast<double>(N) * static_cast<double>(gh) / static_cast<double>(gw));
    const int cols = static_cast<int>(std::ceil(cols_f));
    const int rows = static_cast<int>(std::ceil(static_cast<double>(N) / static_cast<double>(cols)));

    int w = cols * static_cast<int>(gw);
    int h = rows * static_cast<int>(gh);

    return {w, h};
}

inline void write_font_cache(const Asura::Font& font, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening font cache for writing: " << path << '\n';
        return;
    }

    uint64_t bitmap_size = font.bitmap.size();
    out.write(reinterpret_cast<const char*>(&bitmap_size), sizeof(bitmap_size));

    if (bitmap_size > 0) {
        out.write(reinterpret_cast<const char*>(font.bitmap.data()), static_cast<std::streamsize>(bitmap_size));
    }

    out.write(reinterpret_cast<const char*>(font.chars.data()), static_cast<std::streamsize>(NUM_CHARS * sizeof(stbtt_bakedchar)));
}


inline bool read_font_cache(Asura::Font& font, const std::string& path, std::size_t expected_bitmap_size) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening font cache for reading: " << path << '\n';
        return false;
    }

    uint64_t bitmap_size = 0;
    in.read(reinterpret_cast<char*>(&bitmap_size), sizeof(bitmap_size));
    if (!in) {
        Asura::log().error("Bad read on bitmap size for {} from: {}", font.name, path);
        return false;
    }

    if (bitmap_size != expected_bitmap_size) {
        Asura::log().error("Bitmap size mismatch in cache for font {} at: {} (expected {}, got {})", 
            font.name, path, expected_bitmap_size, bitmap_size);
        return false;  // fallback to regenerate
    }

    font.bitmap.resize(bitmap_size);
    if (bitmap_size > 0) {
        in.read(reinterpret_cast<char*>(font.bitmap.data()),
                static_cast<std::streamsize>(bitmap_size));
        if (!in) {
            Asura::log().error("Bad read on bitmap for {} from: {}", font.name, path);
            return false;
        }
    }

    // read baked chars
    in.read(reinterpret_cast<char*>(font.chars.data()),
            static_cast<std::streamsize>(NUM_CHARS * sizeof(stbtt_bakedchar)));
    if (!in) {
        Asura::log().error("Bad read on baked chars for {} from: {}", font.name, path);
        return false;
    }

    return true;
}


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
        
        sg_update_buffer(vbuf, SG_VECTOR_RANGE(verts));

        sg_bindings bind = {};
        bind.vertex_buffers[0] = vbuf;
        bind.index_buffer = ibuf;
        bind.views[VIEW_text_tex] = f.view;
        bind.samplers[SMP_text_smp] = smp;

        sg_apply_bindings(&bind);
        sg_apply_uniforms(UB_text_params, SG_RANGE(vs_params));
        sg_draw(0, static_cast<int>(indices.size()), 1);
    }

    _clear();
}

void Asura::FontRenderer::_clear() {
    for (auto& f : fonts) {
        f.batch.verts.clear();
        f.batch.indices.clear();
    }
}

void Asura::FontRenderer::_init_fonts(const char *dir) {
    fonts.clear();
    fonts.reserve(kFontDefs.size());

    ordered_json j;

    j["first_char"] = FIRST_CHAR;
    j["num_chars"]  = NUM_CHARS;
    j["names_hash"] = std::to_string(compute_resource_hash(kFontDefs));
    j["fonts"] = ordered_json::object();
    
    std::string json_path = join_path_json(dir, "fonts");

    bool rewrite_json = false;

    for (auto& [name, id, size] : kFontDefs) {
        Font font = {};
        font.id   = id;
        font.name = name;
        font.size = size;

        std::string png = join_path_png(dir, font.name);
        std::string ttf = join_path_ttf(dir, font.name);
        std::string bin = join_path_bin(dir, font.name);
        
        if (!std::filesystem::exists(png) || !std::filesystem::exists(json_path)) {
            std::size_t ttf_size;
            std::uint8_t* ttf_data = read_file(ttf.c_str(), ttf_size);
            
            auto [gw, gh] = estimateGlyphCellSize(ttf_data, static_cast<float>(font.size), FIRST_CHAR, NUM_CHARS);
            auto [atlas_w, atlas_h] = estimateAtlasSize(gw, gh, NUM_CHARS);

            font.w = std::bit_ceil(static_cast<uint32_t>(atlas_w));
            font.h = std::bit_ceil(static_cast<uint32_t>(atlas_h));

            font.bitmap.resize(font.w * font.h);

            int ret = stbtt_BakeFontBitmap(
                ttf_data,
                0,
                static_cast<float>(size),
                font.bitmap.data(),
                font.w, font.h,
                FIRST_CHAR, NUM_CHARS,
                font.chars.data()
            );
            if (ret == 0) die("Failed to load font at: " + ttf);

            stbi_write_png(png.c_str(), font.w, font.h, 1, font.bitmap.data(), font.w);
            
            write_font_cache(font, bin);

            j["fonts"][font.name] = {
                {"bin", bin},
                {"png", png},
                {"w", font.w}, {"h", font.h},
                {"size", font.bitmap.size()},
                {"pixel_size", font.size}
            };

            log().info("Generated bitmap font \033[1m{}\033[0m (enum id={})", name, id);
        } else {
            json data;
            if (read_json_file(json_path, data)) {
                try {
                    auto f = data["fonts"][font.name];
                    std::size_t size = f["size"].get<std::size_t>();
                    std::string bin_path = f["bin"];
                    
                    font.w = f.at("w").get<int>();
                    font.h = f.at("h").get<int>();

                    if (!read_font_cache(font, bin_path, size)) {
                        // fallback: regenerate this font (bake again, overwrite bin/json)
                        // for now log and skip
                        log().info("Failed to read from {} font cache, bake font again.", font.name);
                    }
                } catch (const std::exception& e) {
                    log().error("JSON error: {}", e.what());
                }
            }

            log().info("Reused bitmap font \033[1m{}\033[0m from metadata (enum id={})", name, id);
        }

        sg_image_desc img = {};
        img.width  = font.w;
        img.height = font.h;
        img.pixel_format = SG_PIXELFORMAT_R8;
        img.usage.immutable = true;
        img.data.mip_levels[0].ptr  = font.bitmap.data();
        img.data.mip_levels[0].size = font.bitmap.size();
        font.atlas = sg_make_image(&img);

        sg_view_desc vd = {};
        vd.texture.image = font.atlas;
        font.view = sg_make_view(&vd);

        int idx = (int)fonts.size();
        fonts.push_back(std::move(font));

        if (id >= 0 && id < (int)id_to_font_index.size()) {
            id_to_font_index[id] = idx;
        }
    } // for (auto& [name, id, size] : kFontDefs)

    if (rewrite_json) write_json_file(json_path, j);
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
    ib.data.ptr = tmp.data();
    ib.data.size = tmp.size() * sizeof(uint16_t);
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

Asura::Font* Asura::FontRenderer::_find_font(int id) {
    if (id < 0 || id >= static_cast<int>(id_to_font_index.size())) return nullptr;
    int idx = id_to_font_index[id];
    if (idx < 0 || idx >= static_cast<int>(fonts.size())) return nullptr;
    return &fonts[idx];
}

void Asura::FontRenderer::_queue_text(int id, std::string_view text, glm::vec2 pos, float scale, sg_color tint) {
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
        stbtt_GetBakedQuad(font->chars.data(), font->w, font->h, ci, &x, &y, &q, true);

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
