//
// Created by Shreejit Murthy on 10/11/2025.
//

#include "font.hh"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "stb_image_write.h"

#include "utils.h"

void asura::FontRenderer::init(const std::string &fonts_dir, std::vector<ResourceDef> reg) {
    kFontDefs = reg;
    _init_fonts(fonts_dir.c_str());
}

void asura::FontRenderer::_init_fonts(const char* dir) {
    const int font_bitmap_w = 256;
    const int font_bitmap_h = 256;

    char_data.resize(96);

    std::vector<unsigned char> bitmap(font_bitmap_w * font_bitmap_h);
    int font_height = 15;  // we'll only load a font we know to be 16px for now
    size_t ttf_size;
    auto ttf_data = read_file(dir, ttf_size);
    auto ret = stbtt_BakeFontBitmap(ttf_data, 0, (float)font_height, bitmap.data(), font_bitmap_w, font_bitmap_h, 32, 96, &char_data[0]);
    if (ret == 0) die(std::format("Failed to load font at {}", dir));

    stbi_write_png("font.png", font_bitmap_w, font_bitmap_h, 1, bitmap.data(), font_bitmap_w);
}
