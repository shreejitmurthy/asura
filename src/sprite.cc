//
// Created by Shreejit Murthy on 28/10/2025
//

#include "sprite.hh"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_rect_pack.h>

#include <nlohmann/json.hpp>
using namespace nlohmann;

#include "core/utils.h"
#include "core/log.h"

#define offsetir(v) (int)offsetof(InstanceData, v)

inline static sg_buffer make_unit_vbuf() {
    const float vertices[] = {
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 1.0f,   0.0f, 1.0f
    };
    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage.vertex_buffer = true;
    vbuf_desc.usage.immutable = true;
    vbuf_desc.label = "vertices";
    return sg_make_buffer(&vbuf_desc);
}

inline static sg_buffer make_ibuf() {
    const uint16_t indices[] = { 0, 1, 2,  2, 3, 0 };
    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.usage.index_buffer = true;
    ibuf_desc.label = "indices";
    return sg_make_buffer(&ibuf_desc);
}


void Asura::SpriteRenderer::init(const std::string &images_dir, std::vector<ResourceDef> reg) {
    kSpriteDefs = reg;
    _init_images(images_dir.c_str());
    _init_ir(atlas.path);
}

void Asura::SpriteRenderer::render(Math::Mat4 projection, Math::Mat4 view) {
    _update_ir(projection, view);
    _draw_ir();
    _clear();
}

void Asura::SpriteRenderer::_pack(const PackDef& def) {
    int sizeX = def.sizeX;
    int sizeY = def.sizeY;
    std::vector<stbrp_rect> rects = def.rects;
    const std::string& out_dir = def.out_dir;
    int rect_count = def.rect_count;
    int awidth     = def.awidth;
    int aheight    = def.aheight;

    atlas.width  = awidth  >= 8192 ? 8192 : awidth;
    atlas.height = aheight >= 8192 ? 8192 : aheight;
    
    stbrp_context ctx;
    std::vector<stbrp_node> nodes(static_cast<size_t>(atlas.width));
    stbrp_init_target(&ctx, atlas.width, atlas.height, nodes.data(), atlas.width);

    int ok = stbrp_pack_rects(&ctx, rects.data(), rect_count);
    if (!ok) { die("Failed to pack all rects"); }

    // create atlas pixel buffer
    const size_t stride = static_cast<size_t>(atlas.width) * 4;
    std::vector<unsigned char> raw_data(static_cast<size_t>(atlas.height) * stride, 0);

    atlas.path = join_path_png(out_dir, "atlas");

    ordered_json j;
    j["width"]       = atlas.width;
    j["height"]      = atlas.height;
    j["true_width"]  = sizeX;
    j["true_height"] = sizeY;
    j["rect_count"]  = rect_count;
    j["names_hash"]  = std::to_string(compute_resource_hash(kSpriteDefs));
    j["sprites"]     = ordered_json::object();

    // blit each rect and fill json
    for (int i = 0; i < rect_count; ++i) {
        const stbrp_rect& rec = rects[static_cast<size_t>(i)];
        Sprite& tex = sprites[rec.id];
        tex.x = rec.x;
        tex.y = rec.y;

        j["sprites"][tex.name] = {
            {"x", tex.x}, {"y", tex.y},
            {"w", tex.width}, {"h", tex.height}
        };

        for (int row = 0; row < rec.h; ++row) {
            const unsigned char* src_row = tex.data + (static_cast<size_t>(row) * static_cast<size_t>(rec.w) * 4);
            unsigned char* dest_row = raw_data.data() +
               (static_cast<size_t>(rec.y + row) * static_cast<size_t>(atlas.width) + static_cast<size_t>(rec.x)) * 4;
            std::memcpy(dest_row, src_row, static_cast<size_t>(rec.w) * 4);
        }
        if (tex.data) { stbi_image_free(tex.data); tex.data = nullptr; }
    }

    if (!stbi_write_png(atlas.path.c_str(), atlas.width, atlas.height, 4, raw_data.data(), atlas.width * 4)) {
        die("Failed to write out atlas");
    }

    write_json_file(join_path_json(out_dir, "atlas"), j);
    log().info("Packaged images into: {}", atlas.path);
}

void Asura::SpriteRenderer::_pack_images(const std::string &out_dir) {
    int rect_count = 0;
    for (int id = 0; id < sprite_count; ++id) if (sprites[id].width > 0) ++rect_count;
    if (rect_count == 0) return;

    std::vector<stbrp_rect> rects(static_cast<size_t>(rect_count));
    int k = 0, sizeX = 0, sizeY = 0;
    for (int id = 0; id < sprite_count; ++id) {
        Sprite& tex = sprites[id];
        if (tex.width == 0) continue;
        rects[k].id = id;
        rects[k].w  = tex.width;
        rects[k].h  = tex.height;
        rects[k].x  = rects[k].y = 0;
        rects[k].was_packed = 0;
        sizeX += tex.width;
        sizeY += tex.height;
        ++k;
    }

    const uint64_t awidth  = std::bit_ceil(static_cast<uint64_t>(sizeX));
    const uint64_t aheight = std::bit_ceil(static_cast<uint64_t>(sizeY));

    const std::string json_path = join_path_json(out_dir, "atlas");
    const std::string png_path  = join_path_png(out_dir,  "atlas");
    json data;

    const bool have_meta = std::filesystem::exists(json_path) && read_json_file(json_path, data);

    bool can_reuse = false;
    if (have_meta) {
        const int  j_rect_count  = data.value("rect_count", -1);
        const int  j_true_w      = data.value("true_width", -1);
        const int  j_true_h      = data.value("true_height", -1);
        const auto j_names_hash  = data.value("names_hash", std::string{});
        const auto cur_hash      = std::to_string(compute_resource_hash(kSpriteDefs));

        if (j_rect_count == rect_count &&
            j_true_w == sizeX &&
            j_true_h == sizeY &&
            j_names_hash == cur_hash &&
            std::filesystem::exists(png_path) &&
            data.contains("sprites") && data["sprites"].is_object())
        {
            // Ensure every sprite has an entry
            can_reuse = true;
            for (int id = 0; id < sprite_count && can_reuse; ++id) {
                Sprite& s = sprites[id];
                if (s.width == 0) continue;
                if (!data["sprites"].contains(s.name)) {
                    can_reuse = false;
                    break;
                }
            }
        }
    }

    if (!can_reuse) {
        _pack({sizeX, sizeY, rects, out_dir, rect_count, (int)awidth, (int)aheight});
    } else {
        atlas.path   = png_path;
        atlas.width  = data.value("width",  0);
        atlas.height = data.value("height", 0);

        for (int id = 0; id < sprite_count; ++id) {
            Sprite& s = sprites[id];
            if (s.width == 0) continue;
            auto& js = data["sprites"][s.name];
            s.x = js.value("x", 0);
        }
        log().info("Reused atlas from metadata: {}", atlas.path);
    }
}


void Asura::SpriteRenderer::_init_images(const char *dir) {
    sprite_count = 0;
    int highest_id = 0;

    sprites.clear();
    sprites.resize(128);

    for (size_t i = 0; i < kSpriteDefs.size(); ++i) {
        const int id = kSpriteDefs[i].id;
        if (id == 0) continue;
        if (id < 0 || id >= (int)sprites.size()) continue;
        if (id > highest_id) highest_id = id;

        auto png = join_path_png(dir, kSpriteDefs[i].name);

        int w = 0, h = 0, n = 0;
        unsigned char* img = stbi_load(png.c_str(), &w, &h, &n, 4);
        if (!img) die(std::format("Failed to load image at: {}", png));
        Sprite tex = {};
        tex.width = w; tex.height = h; tex.channels = 4; tex.data = img;
        tex.name = kSpriteDefs[i].name;
        sprites[id] = tex;
        log().info("Loaded image \033[1m{}\033[0m (enum id={})", kSpriteDefs[i].name, kSpriteDefs[i].id);
    }

    sprite_count = highest_id + 1;
    _pack_images(dir);
}

// For some reason this throws an error if I don't do Asura::Math instead of just Math
inline Asura::Math::Vec2 get_tile_uv(Asura::Math::Vec2 tile_index, Asura::Math::Vec2 tile_size, Asura::Math::Vec2 atlas_size) {
    return {
        tile_index.x * tile_size.x / (atlas_size.x),
        tile_index.y * tile_size.y / (atlas_size.y)
    };
}

void Asura::SpriteRenderer::_init_ir(const std::string& path) {
    ir.vs_params.mvp = get_default_projection();

    sg_shader shader = sg_make_shader(instance_shader_desc(sg_query_backend()));
    int w, h;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, NULL, 4);

    sg_image_desc img_desc = {};
    img_desc.width  = w;
    img_desc.height = h;
    img_desc.data.mip_levels[0].ptr = pixels;
    img_desc.data.mip_levels[0].size = static_cast<size_t>(w * h * 4);
    sg_image image = sg_make_image(&img_desc);

    stbi_image_free(pixels);

    ir.width  = w;
    ir.height = h;

    sg_sampler_desc smp_desc = {};
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_NEAREST;
    sg_sampler smp = sg_make_sampler(&smp_desc);

    ir.bindings.vertex_buffers[0] = make_unit_vbuf();
    ir.bindings.index_buffer = make_ibuf();
    sg_view_desc view_desc = {};
    view_desc.texture.image = image;
    ir.bindings.views[VIEW_inst_tex] = sg_make_view(&view_desc);

    ir.bindings.samplers[SMP_inst_smp] = smp;

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.size = sizeof(InstanceData) * MAX_INSTANCES;
    vbuf_desc.usage.stream_update = true;
    vbuf_desc.usage.vertex_buffer = true;
    vbuf_desc.label = "instance-buffer";
    ir.bindings.vertex_buffers[1] = sg_make_buffer(&vbuf_desc);

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shader;
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pip_desc.layout.buffers[0].stride = 4 * sizeof(float);
    pip_desc.layout.buffers[1].stride = sizeof(InstanceData);
    pip_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
    // attrs follows buffer_idx, offset, format
    pip_desc.layout.attrs[ATTR_instance_aPos] = { 0, 0, SG_VERTEXFORMAT_FLOAT2 };
    pip_desc.layout.attrs[ATTR_instance_aUV]  = { 0, 2 * (int)sizeof(float), SG_VERTEXFORMAT_FLOAT2 };

    pip_desc.layout.attrs[ATTR_instance_aOffset]     = { 1, offsetir(offset),     SG_VERTEXFORMAT_FLOAT2, };
    pip_desc.layout.attrs[ATTR_instance_aUVOffset]   = { 1, offsetir(uvOffset),   SG_VERTEXFORMAT_FLOAT2, };
    pip_desc.layout.attrs[ATTR_instance_aWorldScale] = { 1, offsetir(worldScale), SG_VERTEXFORMAT_FLOAT2, };
    pip_desc.layout.attrs[ATTR_instance_aUVScale]    = { 1, offsetir(uvScale),    SG_VERTEXFORMAT_FLOAT2, };
    pip_desc.layout.attrs[ATTR_instance_aRotation]   = { 1, offsetir(rotation),   SG_VERTEXFORMAT_FLOAT,  };
    pip_desc.layout.attrs[ATTR_instance_aPivot]      = { 1, offsetir(pivot),      SG_VERTEXFORMAT_FLOAT2, };
    pip_desc.layout.attrs[ATTR_instance_aTint]       = { 1, offsetir(tint),       SG_VERTEXFORMAT_FLOAT4, };

    pip_desc.colors[0].blend.enabled          = true;
    pip_desc.colors[0].blend.src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.colors[0].blend.dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_rgb           = SG_BLENDOP_ADD;
    pip_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.colors[0].blend.op_alpha         = SG_BLENDOP_ADD;

    pip_desc.label = "instance-pipeline";

    ir.pipeline = sg_make_pipeline(&pip_desc);

    ir.instances.clear();
}

void Asura::SpriteRenderer::_push_instance(int id, Math::Vec2 position, Math::Vec2 scale, float rotation, Math::Vec2 pivot, Math::Vec2 pivot_px, sg_color tint) {
    if (ir.instances.size() < MAX_INSTANCES) {
        Sprite& tex = sprites[id];
        ir.instances.push_back(_create_instance_data({tex, position, scale, rotation, pivot, pivot_px, tint}));
    }
}

void Asura::SpriteRenderer::_update_ir(Math::Mat4 projection, Math::Mat4 view) {
    sg_range range = { .ptr = ir.instances.data(), .size = ir.instances.size() * sizeof(InstanceData) };
    sg_update_buffer(ir.bindings.vertex_buffers[1], &range);
    ir.dirty = false;
    ir.vs_params.mvp = projection * view;
}

void Asura::SpriteRenderer::_draw_ir() const {
    if (ir.instances.size() == 0) return;
    sg_apply_pipeline(ir.pipeline);
    sg_apply_bindings(&ir.bindings);
    sg_apply_uniforms(UB_instance_params, SG_RANGE(ir.vs_params));

    sg_draw(0, 6, ir.instances.size());
}
