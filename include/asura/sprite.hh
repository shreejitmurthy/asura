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
#include <sokol/sokol_app.h>

#include <sokol/util/sokol_color.h>

#include <stb_rect_pack.h>

#include "shaders/shader.glsl.h"

#include "resource.hh"

#define MAX_INSTANCES (1024)

inline static glm::mat4 get_default_projection() {
    return glm::ortho(0.f, sapp_widthf() / sapp_dpi_scale(), sapp_heightf() / sapp_dpi_scale(), 0.f, -1.f, 1.f);
}

typedef struct {
    int width, height, channels;
    int x, y;
    unsigned char* data;
    // vec4 atlas_uvs;
    const char* name;
} Sprite;

typedef struct {
    int width, height;
    std::string path;
} SpriteAtlas;

typedef struct {
    glm::vec2 offset;
    glm::vec2 uvOffset;
    glm::vec2 worldScale;
    glm::vec2 uvScale;
    float rotation;
    glm::vec2 pivot;
    glm::vec4 tint;
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
    static glm::vec2 Centre()  { return {0.5, 0.5}; }
    static glm::vec2 TopLeft() { return {0.0, 0.0}; }
};

// Sprite
class SpriteRenderer {
public:
    void init(const std::string& images_dir, std::vector<ResourceDef> reg);

    template <typename E>
        requires std::is_enum_v<E>
    void push(E id, glm::vec2 position, glm::vec2 scale = {1, 1}, float rotation = 0, sg_color tint = sg_white, glm::vec2 pivot = Pivot::TopLeft(), glm::vec2 pivot_px = {0, 0}) {
        _push_instance(std::to_underlying(id), position, scale, rotation, pivot, pivot_px, tint);
    }

    void render(glm::mat4 projection = get_default_projection(), glm::mat4 view = glm::mat4(1.f));

    void clear() { ir.instances.clear(); }
private:
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
        glm::vec2 position;
        glm::vec2 scale;
        float rotation;
        glm::vec2 pivot;
        glm::vec2 pivot_px;
        sg_color tint;
    } InstanceDef;

    InstanceData _create_instance_data(InstanceDef def) {
        const Sprite& tex = def.tex;
        glm::vec2 position = def.position;
        glm::vec2 scale = def.scale;
        float rotation = def.rotation;
        glm::vec2 pivot = def.pivot;
        glm::vec2 pivot_px = def.pivot_px;
        sg_color tint = def.tint;

        InstanceData ret{};

        glm::vec2 sc = scale;
        glm::vec2 pv = pivot;

        if (pivot_px != glm::vec2(0)) {
            pv.x = pivot_px.x / static_cast<float>(tex.width);
            pv.y = pivot_px.y / static_cast<float>(tex.height);
        }

        ret.pivot = pv;

        ret.offset = position;
        ret.worldScale.x = tex.width * sc.x;
        ret.worldScale.y = tex.height * sc.y;

        glm::vec4 tintv;
        if (tint.a == 0.f) {
            tintv = {1, 1, 1, 1};
        } else {
            tintv = {tint.r, tint.g, tint.b, tint.a};
        }

        ret.tint = tintv;

        ret.uvOffset[0] = tex.x / static_cast<float>(ir.width);
        ret.uvOffset[1] = tex.y / static_cast<float>(ir.height);
        ret.uvScale[0]  = tex.width  / static_cast<float>(ir.width);
        ret.uvScale[1]  = tex.height / static_cast<float>(ir.height);
        ret.rotation    = rotation;
        return ret;
    }

    void _init_ir(const std::string& path);
    void _push_instance(int id, glm::vec2 position, glm::vec2 scale, float rotation, glm::vec2 pivot, glm::vec2 pivot_px, sg_color tint);
    void _update_ir(glm::mat4 projection, glm::mat4 view);
    void _draw_ir() const;
};

} // namespace Asura

