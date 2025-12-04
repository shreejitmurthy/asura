//
// Created by Shreejit Murthy on 10/11/2025.
//

#pragma once

#include "resource.hh"

#include <string>
#include <vector>

#include "stb_truetype.h"

namespace asura {

class FontRenderer {
public:
    void init(const std::string& fonts_dir, std::vector<ResourceDef> reg = {});
private:
    void _init_fonts(const char* dir);

    std::vector<ResourceDef> kFontDefs;
    std::vector<stbtt_bakedchar> char_data;
    // std::vector<

};

} // asura
