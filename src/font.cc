//
// Created by Shreejit Murthy on 10/11/2025.
//

#include <utility>

#include "font.hh"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "stb_image_write.h"

#include "utils.h"

void asura::FontRenderer::init(const std::string &fonts_dir, std::vector<ResourceDef> reg) {
    kFontDefs = std::move(reg);
    _init_fonts(fonts_dir.c_str());
}

void asura::FontRenderer::_init_fonts(const char* dir) {
    const int font_bitmap_w = 256;
    const int font_bitmap_h = 256;

    for (size_t i = 0; i < kFontDefs.size(); ++i) {
        const int id = kFontDefs[i].id;
        // if (id < 0 || id >= static_cast<int>(fonts.size())) continue;
        std::vector<unsigned char> bitmap(font_bitmap_w * font_bitmap_h);
        std::size_t ttf_size;
        auto ttf_data = read_file(join_path_ttf(dir, kFontDefs[i].name).c_str(), ttf_size);
        auto ret = stbtt_BakeFontBitmap(ttf_data, 0, static_cast<float>(kFontDefs[i].size) - 1, bitmap.data(), font_bitmap_w, font_bitmap_h, 32, 96, &char_data[0]);
        if (ret == 0) die(std::format("Failed to load font at {}", join_path_ttf(dir, kFontDefs[i].name)));

        stbi_write_png(join_path_png(dir, kFontDefs[i].name).c_str(), font_bitmap_w, font_bitmap_h, 1, bitmap.data(), font_bitmap_w);
    }

    // char_data.resize(96);
    //
    // std::vector<unsigned char> bitmap(font_bitmap_w * font_bitmap_h);
    // int font_height = 15;  // we only load a font we know to be 16px for now
    // size_t ttf_size;
    // auto ttf_data = read_file(dir, ttf_size);
    // auto ret = stbtt_BakeFontBitmap(ttf_data, 0, static_cast<float>(font_height), bitmap.data(), font_bitmap_w, font_bitmap_h, 32, 96, &char_data[0]);
    // if (ret == 0) die(std::format("Failed to load font at {}", dir));
    //
    // // For now save to build directory, later when we parse directory we can use the join_path_ttf(...)
    // stbi_write_png("font.png", font_bitmap_w, font_bitmap_h, 1, bitmap.data(), font_bitmap_w);
}
