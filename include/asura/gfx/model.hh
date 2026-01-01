//
// Created by Shreejit Murthy on 1/1/2026
//
// Happy New Year :)

#pragma once

namespace Asura::Model {

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