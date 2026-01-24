//
// Created by Shreejit Murthy on 1/1/2026
//
// Happy New Year :)

#pragma once

namespace Asura::Model {

/* 
 * Metal and D3D11 support storage buffers, as well as OpenGL 4.1+.
 * This can very easily be checked at runtime with `sg_query_features().compute`
 * Skeletal is just using the immediate mode debug renderer to draw the lines of the skeleton, 
 * showing the joints and how they move. 
 */
enum class RenderMethod {
    HardwareInstance,
    StorageBuffer,
    Skeletal
};

class Renderer {
public:
    void init();

    void push(RenderMethod rmethod) {
        /* 
         * Skeletal renders are done with immediate-mode OpenGL calls and therefore
         * cannot be instanced or rendered with storage buffers, so we'll just draw the
         * skeleton itself here.
         */
        if (rmethod == RenderMethod::Skeletal) draw_skel();
    }

    void render();
private:
    void init_inst();
    void draw_inst();

    void init_stbuf();
    void draw_stbuf();

    void init_skel(const char* model_path, const char* anim_path);
    void draw_skel();
};

}