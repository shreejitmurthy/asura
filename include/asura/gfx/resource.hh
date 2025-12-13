//
// Created by Shreejit Murthy on 28/10/2025
//

#pragma once

#include <type_traits>
#include <utility>
#include <string>
#include <algorithm>
#include <vector>

namespace Asura {
typedef struct ResourceDef {
    const char* name;
    int id;
    int size;
} ResourceDef;

// Sprite: s.width > 0

// TODO: Needs a better name.
inline std::size_t compute_resource_hash(const std::vector<ResourceDef>& res) {
    std::vector<std::string> names;
    names.reserve(res.size());
    for (auto const& s : res) names.push_back(s.name);
    std::sort(names.begin(), names.end());
    // simple stable hash
    std::size_t h = 1469598103934665603ull;
    for (auto const& n : names) {
        for (unsigned char c : n) { h ^= c; h *= 1099511628211ull; }
        h ^= 0xff; h *= 1099511628211ull;
    }
    return h;
}

} // Asura