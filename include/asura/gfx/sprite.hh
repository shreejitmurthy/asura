//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <type_traits>
#include <utility>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sokol/sokol_gfx.h>

#include <sokol/util/sokol_color.h>

#include <stb_rect_pack.h>

#include "shaders/shader.glsl.h"

#include "resource.hh"
#include "device.hh"

#define MAX_INSTANCES (1024)

inline static Asura::Math::Mat4 get_default_projection() {
    int dpi_scale = Asura::Device::instance().high_dpi ? 2 : 1;
    float w = static_cast<float>(Asura::Device::instance().width);
    float h = static_cast<float>(Asura::Device::instance().height);
    return Asura::Math::Mat4::ortho(0.f, w / dpi_scale, h / dpi_scale, 0.f, -1.f, 1.f);
}

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
    Asura::Math::Vec2 offset;
    Asura::Math::Vec2 uvOffset;
    Asura::Math::Vec2 worldScale;
    Asura::Math::Vec2 uvScale;
    float rotation;
    Asura::Math::Vec2 pivot;
    Asura::Math::Vec4 tint;
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

namespace Asura {
class Pivot {
public:
    /*
        Note: These are not typical NDC. Since Asura flips images on load, the following are vertical opposites
        of the true normalized device coordinates (ie, NDC bottom left => Asura top left).
    */
    static Math::Vec2 Centre()  { return {0.5, 0.5}; }
    static Math::Vec2 TopLeft() { return {0.0, 0.0}; }
};

// Sprite
class SpriteRenderer {
public:
    void init(const std::string& images_dir, std::vector<ResourceDef> reg);

    template <typename E>
        requires std::is_enum_v<E>
    void push(E id, Math::Vec2 position, Math::Vec2 scale = {1, 1}, float rotation = 0, sg_color tint = sg_white, Math::Vec2 pivot = Pivot::TopLeft(), Math::Vec2 pivot_px = {0, 0}) {
        _push_instance(std::to_underlying(id), position, scale, rotation, pivot, pivot_px, tint);
    }

    void render(Math::Mat4 projection = get_default_projection(), Math::Mat4 view = Math::Mat4(1.f));

private:
    void _clear() { ir.instances.clear(); }
    
    std::vector<ResourceDef> kSpriteDefs;
    std::vector<Sprite> sprites;
    InstancedRenderer ir;
    SpriteAtlas atlas;
    int sprite_count;

    typedef struct {
        int sizeX, sizeY;
        std::vector<stbrp_rect> rects;
        const std::string& out_dir;
        int rect_count, awidth, aheight;
    } PackDef;

    void _pack(const PackDef& def);

    void _pack_images(const std::string& out_dir);
    void _init_images(const char* dir);

    typedef struct {
        const Sprite& tex;
        Math::Vec2 position;
        Math::Vec2 scale;
        float rotation;
        Math::Vec2 pivot;
        Math::Vec2 pivot_px;
        sg_color tint;
    } InstanceDef;

    InstanceData _create_instance_data(InstanceDef def) {
        const Sprite& tex = def.tex;
        Math::Vec2 position = def.position;
        Math::Vec2 scale = def.scale;
        float rotation = def.rotation;
        Math::Vec2 pivot = def.pivot;
        Math::Vec2 pivot_px = def.pivot_px;
        sg_color tint = def.tint;

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

        Math::Vec4 tintv;
        if (tint.a == 0.f) {
            tintv = {1, 1, 1, 1};
        } else {
            tintv = {tint.r, tint.g, tint.b, tint.a};
        }

        ret.tint = tintv;

        ret.uvOffset.x = tex.x / static_cast<float>(ir.width);
        ret.uvOffset.y = tex.y / static_cast<float>(ir.height);
        ret.uvScale.x  = tex.width  / static_cast<float>(ir.width);
        ret.uvScale.y  = tex.height / static_cast<float>(ir.height);
        ret.rotation    = rotation;
        return ret;
    }

    void _init_ir(const std::string& path);
    void _push_instance(int id, Math::Vec2 position, Math::Vec2 scale, float rotation, Math::Vec2 pivot, Math::Vec2 pivot_px, sg_color tint);
    void _update_ir(Math::Mat4 projection, Math::Mat4 view);
    void _draw_ir() const;
};

} // Asura

