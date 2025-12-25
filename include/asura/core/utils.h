//
// Created by Shreejit Murthy on 25/8/2025
//

#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

#include <nlohmann/json.hpp>

#include "log.h"
#include "result.hh"
#include "math.hh"

/* GENERAL UTILITIES */
namespace Asura::Utils {

/* TODO
 * Should only be used when developing, remove all instances in code for release.
 * Consider checking for Device BuildMode.
 */
static void die(const std::string& msg) {
    Log::get().critical(msg);
    exit(1);
}

// Consider moving to seperate Resource namespace should more asset-related functions arise.
// Cast to integer for resource definitions
template <typename E>
inline constexpr std::uint8_t rccast(E e) {
    return static_cast<int>(e);
}

} // Asura::Utils





/* SYSTEM UTILITIES (file i/o, paths) */
namespace Asura::Utils::System {
inline std::string join_path_png(std::string_view dir, std::string_view name) {
    std::filesystem::path path(dir);
    path /= name;
    path.replace_extension(".png");
    return path.string();
}

inline std::string join_path_ttf(std::string_view dir, std::string_view name) {
    std::filesystem::path path(dir);
    path /= name;
    path.replace_extension(".ttf");
    return path.string();
}

inline std::string join_path_json(std::string_view dir, std::string_view name) {
    std::filesystem::path path(dir);
    path /= name;
    path.replace_extension(".json");
    return path.string();
}

inline std::string join_path_bin(std::string_view dir, std::string_view name) {
    std::filesystem::path path(dir);
    path /= name;
    path.replace_extension(".bin");
    return path.string();
}

inline bool read_json_file(const std::string& path, nlohmann::json& out) {
    try {
        std::ifstream f(path);
        if (!f) return false;
        out = nlohmann::json::parse(f);
        return true;
    } catch (...) { return false; }
}

inline void write_json_file(const std::string& path, const nlohmann::json& j) {
    std::ofstream f(path);
    if (!f) die("Failed to open json for write");
    f << j.dump(4);
}

inline std::uint8_t* read_file(const char* filename, size_t& size) {
    std::ifstream f(filename, std::ios::binary | std::ios::ate);
    size = f.tellg();
    auto* data = new std::uint8_t[size];
    f.seekg(0);
    f.read(reinterpret_cast<char*>(data), size);
    return data;
}

// Safer file reading
inline std::vector<std::uint8_t> read_file_vec(const char* filename) {
    std::ifstream f(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = f.tellg();
    if (size <= 0) return {};
    f.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> buf(static_cast<size_t>(size));
    if (!f.read(reinterpret_cast<char*>(buf.data()), size)) return {};
    return buf;
}

inline void writeBinary(const std::vector<std::uint8_t>& data, const std::string& path) {
    std::ofstream file(path, std::ios::out | std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    
}

inline std::vector<std::uint8_t> readBinary(const std::string& path, std::size_t size) {
    std::vector<std::uint8_t> data(size);
    std::ifstream(path, std::ios::in | std::ios::binary)
        .read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    return data;
}

inline fs::path findAsuraPath(const fs::path &startDir) {
    fs::path dir = startDir;
    while (true) {
        fs::path potentialPath = dir / "asura";
        if (fs::exists(potentialPath) && fs::is_directory(potentialPath)) {
            return fs::absolute(potentialPath);
        }

        if (dir == dir.root_path()) {
            die("Asura directory not found");
        }

        dir = dir.parent_path();
    }
}

inline Result<std::string, int> findPath(const fs::path& targetPath, const fs::path& startDir = fs::current_path()) {
    fs::path dir = startDir;

    while (true) {
        if (fs::path potentialPath = dir / targetPath; fs::exists(potentialPath)) {
            // return fs::absolute(potentialPath);
            return {fs::absolute(potentialPath)};
        }

        if (dir == dir.root_path()) {
            return {true};
        }

        dir = dir.parent_path();
    }
}

} // Asura::Utils::System





/* GRAPHICS UTILITIES (math functionality) */
namespace Asura::Utils::Gfx {

inline static Math::Mat4 get_default_projection(int dpi_scale) {
    auto w = static_cast<float>(Asura::Device::instance().width);
    auto h = static_cast<float>(Asura::Device::instance().height);
    return Math::Mat4::ortho(0.f, w / dpi_scale, h / dpi_scale, 0.f, -1.f, 1.f);
}

inline static void update_projection_matrix(Math::Vec2 dim, Math::Vec2 virtual_dim, Math::Mat4& projection) {
    float window_w = dim.x;
    float window_h = dim.y;

    const float virtual_w = virtual_dim.x;
    const float virtual_h = virtual_dim.y;

    // Compute aspect ratios
    float aspect_virtual = virtual_w / virtual_h;
    float aspect_window  = window_w / window_h;

    float scale;
    float offset_x = 0.0f;
    float offset_y = 0.0f;

    if (aspect_window > aspect_virtual) {
        // window is wider than desired: letterbox
        scale    = window_h / virtual_h;
        offset_x = (window_w - virtual_w * scale) * 0.5f;
    } else {
        // window is taller than desired: pillarbox
        scale    = window_w / virtual_w;
        offset_y = (window_h - virtual_h * scale) * 0.5f;
    }

    using namespace Math;

    Mat4 proj  = Mat4::ortho(0.0f, window_w, window_h, 0.0f, -1.0f, 1.0f);
    Mat4 model = Mat4::translate(Vec3(offset_x, offset_y, 0.0f)) * Mat4::scale(Vec3(scale, scale, 1.0f));

    projection = proj * model;
}

} // Asura::Utils::Gfx
