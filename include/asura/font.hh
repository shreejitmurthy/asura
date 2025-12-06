//
// Created by Shreejit Murthy on 10/11/2025.
//

#pragma once

#include "resource.hh"

#include <string>
#include <vector>

#include "stb_truetype.h"

static constexpr int FIRST_CHAR = 32;
static constexpr int NUM_CHARS  = 96;

namespace asura {

class FontRenderer {
public:
    void init(const std::string& fonts_dir, std::vector<ResourceDef> reg = {});
private:
    void _init_fonts(const char* dir);

    std::vector<ResourceDef> kFontDefs;
    std::array<stbtt_bakedchar, NUM_CHARS> char_data;
};

} // asura
