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

enum class BuildMode {
    Debug,
    Release,
};

class Device {
public:
    static Device& instance() {
        static Device inst;
        return inst;
    }

    // TODO: Clarify high-dpi because I'm confused.
    Device& init(int w, int h, const std::string& title, bool high_dpi = false) {
        this->width    = w;
        this->height   = h;
        this->name     = title;
        this->high_dpi = high_dpi;
        return *this;
    }

    Device& set_window_backend(const WindowBackend& backend) {
        win_backend = backend;
        return *this;
    }

    Device& set_build_mode(const BuildMode& mode) {
        build_mode = mode;
        switch (mode) {
            case BuildMode::Debug:
                debug = true;
                break;
            default:  // TODO: Relevant Release stuff here.
                debug = false;
                break;
        }
        return *this;
    }

    Device& set_debug_size(int size) {
        debug_scale = size;
        return *this;
    }

    int width;
    int height;
    std::string name;
    bool high_dpi;
    WindowBackend win_backend;
    BuildMode build_mode;
    bool debug = false;
    int debug_scale = 1;

private:
    Device() = default;
};

} // Asura