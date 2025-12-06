//
// Created by Shreejit Murthy on 10/11/2025.
//

#include <utility>

#include "font.hh"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "stb_image_write.h"

#include "utils.h"
#include "log.h"

void Asura::FontRenderer::init(const std::string &fonts_dir, std::vector<ResourceDef> reg) {
    kFontDefs = std::move(reg);
    _init_fonts(fonts_dir.c_str());
}

void Asura::FontRenderer::_init_fonts(const char* dir) {
    const int font_bitmap_w = 256;
    const int font_bitmap_h = 256;

    for (size_t i = 0; i < kFontDefs.size(); ++i) {
        auto filename = join_path_png(dir, kFontDefs[i].name);
        if (!std::filesystem::exists(filename)) {
            std::vector<unsigned char> bitmap(font_bitmap_w * font_bitmap_h);
            std::size_t ttf_size;
            auto ttf_data = read_file(join_path_ttf(dir, kFontDefs[i].name).c_str(), ttf_size);
            auto ret = stbtt_BakeFontBitmap(
                ttf_data,
                0,
                static_cast<float>(kFontDefs[i].size) - 1,
                bitmap.data(),
                font_bitmap_w,
                font_bitmap_h,
                FIRST_CHAR,
                NUM_CHARS,
                char_data.data()
            );
            if (ret == 0) die(std::format("Failed to load font at: {}", join_path_ttf(dir, kFontDefs[i].name)));
    
            stbi_write_png(filename.c_str(), font_bitmap_w, font_bitmap_h, 1, bitmap.data(), font_bitmap_w);
            log().info("Generated bitmap font: {}", filename);
        } else {
            log().info("Reused bitmap font: {}", filename);
        }
    }
}


