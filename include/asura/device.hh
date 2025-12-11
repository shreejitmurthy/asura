//
// Created by Shreejit Murthy on 11/12/2025
//

#pragma once
#include <string>

namespace Asura {

enum class WindowBackend {
    SAPP,
    SDL, GLFW  // both should work the same
};

class Device {
public:
    static Device& instance() {
        static Device inst;
        return inst;
    }

    Device& init(int w, int h, const char* title, bool hi_dpi = false, int debug = 1) {
        width = w;
        height = h;
        name = title;
        high_dpi = hi_dpi;
        debug_scale = debug;
        return *this;
    }

    Device& set_window_backend(const WindowBackend& backend) {
        win_backend = backend;
        return *this;
    }

    int width;
    int height;
    std::string name;
    bool high_dpi;
    int debug_scale;
    WindowBackend win_backend;

private:
    Device() = default;
};

} // Asura