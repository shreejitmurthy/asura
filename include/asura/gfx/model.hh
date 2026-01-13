//
// Created by Shreejit Murthy on 1/1/2026
//
// Happy New Year :)

#pragma once

namespace Asura::Model {

// Metal and D3D11 support storage buffers, as well as OpenGL 4.1+.
// This can very easily be checked at runtime with `sg_query_features().compute`
enum class RenderMethod {
    HardwareInstance,
    StorageBuffer
};

class Renderer {
public:
    void init();

    void push();

    void render();
private:
    void init_inst();
    void draw_inst();

    void init_stbuf();
    void draw_stbuf();

    RenderMethod rmethod;
};

}