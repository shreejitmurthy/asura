//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <type_traits>
#include <utility>
#include <string>

#include <sokol/sokol_gfx.h>

#include <sokol/util/sokol_color.h>

#include <stb_rect_pack.h>

#include "shaders/shader.glsl.h"

#include "resource.hh"
#include "device.hh"
#include "../core/utils.h"

#define MAX_INSTANCES (1024)

namespace Asura::Sprite {

typedef struct {
    int width, height, channels;
    int x, y;
    unsigned char* data;
    // Vec4 atlas_uvs;
    const char* name;
} Sprite;

typedef struct {
    int width, height;
    std::string path;
} SpriteAtlas;

typedef struct {
    Math::Vec2 offset;
    Math::Vec2 uvOffset;
    Math::Vec2 worldScale;
    Math::Vec2 uvScale;
    float rotation;
    Math::Vec2 pivot;
    Math::Vec4 tint;
} InstanceData;

typedef struct InstancedRenderer {
    int width, height;
    instance_params_t vs_params;
    std::vector<InstanceData> instances;
    // size_t instance_count;
    bool dirty;
    sg_bindings bindings;
    sg_pipeline pipeline;
} InstancedRenderer;

class Pivot {
public:
    /*
     * Note: These are not typical NDC. Since Asura flips images on load, the following are vertical opposites
     * of the true normalized device coordinates (ie, NDC bottom left => Asura top left).
     */
    static Math::Vec2 Centre()  { return {0.5, 0.5}; }
    static Math::Vec2 TopLeft() { return {0.0, 0.0}; }
};

// Sprite
class Renderer {
public:
    void init(const std::string& images_dir, std::vector<ResourceDef> reg);

    template <typename E>
    requires std::is_enum_v<E>
    void push(E id, 
            Math::Vec2 position,
            Math::Vec2 scale = {1, 1}, float rotation = 0,
            Math::Vec4 tint = {1, 1, 1, 1},
            Math::Vec2 pivot = Pivot::TopLeft(), Math::Vec2 pivot_px = {0, 0}) 
    {
        _push_instance(std::to_underlying(id), position, scale, rotation, pivot, pivot_px, tint);
    }

    template <typename E>
    requires std::is_enum_v<E>
    void push(E id, 
            Math::Vec2 position,
            Math::Vec2 scale, float rotation,
            sg_color tint = sg_white,
            Math::Vec2 pivot = Pivot::TopLeft(), Math::Vec2 pivot_px = {0, 0}) 
    {
        Math::Vec4 tintv = {tint.r, tint.g, tint.b, tint.a};
        _push_instance(std::to_underlying(id), position, scale, rotation, pivot, pivot_px, tintv);
    }

    void render(Math::Mat4 view = Math::Mat4(1.f));

    void resize(Math::Vec2 dim, Math::Vec2 virtual_dim) { Utils::Gfx::update_projection_matrix(dim, virtual_dim, ir.vs_params.mvp); }

private:
    void _clear() { ir.instances.clear(); }
    
    std::vector<ResourceDef> kSpriteDefs;
    std::vector<Sprite> sprites;
    InstancedRenderer ir = {};
    SpriteAtlas atlas = {};
    int sprite_count = 0;

    typedef struct {
        int sizeX, sizeY;
        std::vector<stbrp_rect> rects;
        const std::string& out_dir;
        int rect_count, awidth, aheight;
    } PackDef;

    void _pack(const PackDef& def);

    void _pack_images(const std::string& out_dir);
    void _init_images(const char* dir);

    typedef struct InstanceDef {
        const Sprite& tex;
        Math::Vec2 position;
        Math::Vec2 scale;
        float rotation = 0.f;
        Math::Vec2 pivot;
        Math::Vec2 pivot_px;
        Math::Vec4 tint;
    } InstanceDef;

    InstanceData _create_instance_data(const InstanceDef &def) {
        const Sprite& tex = def.tex;
        auto position = def.position;
        auto scale = def.scale;
        auto rotation = def.rotation;
        auto pivot = def.pivot;
        auto pivot_px = def.pivot_px;
        auto tint = def.tint;

        InstanceData ret{};

        Math::Vec2 sc = scale;
        Math::Vec2 pv = pivot;

        if (pivot_px != Math::Vec2(0, 0)) {
            pv.x = pivot_px.x / static_cast<float>(tex.width);
            pv.y = pivot_px.y / static_cast<float>(tex.height);
        }

        ret.pivot = pv;

        ret.offset = position;
        ret.worldScale.x = tex.width * sc.x;
        ret.worldScale.y = tex.height * sc.y;

        ret.tint = tint;

        ret.uvOffset.x = tex.x / static_cast<float>(ir.width);
        ret.uvOffset.y = tex.y / static_cast<float>(ir.height);
        ret.uvScale.x  = tex.width  / static_cast<float>(ir.width);
        ret.uvScale.y  = tex.height / static_cast<float>(ir.height);
        ret.rotation   = rotation;
        return ret;
    }

    void _init_ir(const std::string& path);
    void _push_instance(int id, Math::Vec2 position, Math::Vec2 scale, float rotation, Math::Vec2 pivot, Math::Vec2 pivot_px, Math::Vec4 tint);
    void _update_ir(Math::Mat4 projection, Math::Mat4 view);
    void _draw_ir() const;
};

} // Asura

