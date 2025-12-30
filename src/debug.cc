//
// Created by Shreejit Murthy on 30/12/2025
//

#include "debug.hh"

#include "resource.hh"

/* TODO
 * This code is a bit chopped.
 * Handling colours is done very poorly.
 * Consider refactoring at some point.
*/

static void prep_color_and_font(sg_color color, int font, float alpha = 1) {
    sdtx_font(font);
    sdtx_color4b(color.r * 255, color.g * 255, color.b * 255, alpha * 255);
}

// Must prep color before doing this.
static void print_vector(const std::vector<std::string>& text, const std::string& label, sg_color color, int font, float alpha = 1.0f) {
    prep_color_and_font(color, font, alpha);

    sdtx_printf("( ");
    prep_color_and_font(color, font, alpha * 0.5f);
    sdtx_printf("%s\n\n", label.c_str());

    prep_color_and_font(color, font, alpha);
    for (const auto& i : text) {
        sdtx_printf("  %s\n", i.c_str());
    }

    sdtx_printf("\n)\n\n");
}


void Asura::Debug::print(const std::string& text, sg_color color, int font) {
    if (!Device::instance().debug) return;
    prep_color_and_font(color, font);
    sdtx_printf("%s\n\n", text.c_str());
}

void Asura::Debug::print(const std::vector<std::string>& text, const std::string& label, sg_color color, int font) {
    if (!Device::instance().debug) return;
    prep_color_and_font(color, font);
    print_vector(text, label, color, font);
}

void Asura::Debug::temp(const std::string& text, float lifespan, float dt, sg_color color, int font) {
    if (!Device::instance().debug) return;
    static std::string current_message;
    static float remaining = 0.f;

    if (text != current_message) {
        current_message = text;
        remaining = lifespan;
    }

    if (remaining <= 0.015) {
        remaining = 0.f;
    } else {
        remaining -= dt;
        float alpha = remaining / lifespan;
        prep_color_and_font(color, font, alpha);
        sdtx_printf("%s\n\n", text.c_str());
    }
}

void Asura::Debug::temp(const std::vector<std::string>& text, float lifespan, float dt, const std::string& label, sg_color color, int font) {
    if (!Device::instance().debug) return;
    static std::vector<std::string> current_message;
    static float remaining = 0.f;

    if (text != current_message) {
        current_message = text;
        remaining = lifespan;
    }

    if (remaining <= 0.015f) {
        remaining = 0.f;
    } else {
        remaining -= dt;
        float alpha = remaining / lifespan;
        print_vector(text, label, color, font, alpha);
    }
}

void Asura::Debug::resize(Math::Vec2 dim) {
    auto device = Device::instance();
    if (!device.debug) return;
    float d = static_cast<float>(device.debug_scale);
    int dpi_scale = device.high_dpi ? 2 : 1;
    const float cx = dim.x  / dpi_scale * 1 / d;
    const float cy = dim.y  / dpi_scale * 1 / d;
    sdtx_canvas(cx, cy);
    sdtx_origin(1.f, 1.f);
}