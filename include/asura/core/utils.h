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

// Reference: https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
// static uint32_t round_pow2(float v) {
//     uint32_t vi = ((uint32_t) v) - 1;
//     if (vi == 0) return 1;  // clamp to 1
//     for (int shift = 1; shift < 32; shift <<= 1) vi |= vi >> shift;
//     vi++;
//     return vi;
// }

// Should only be used when developing, remove all instances in code for release.
inline static void die(const std::string& msg) {
    Asura::Log::get().critical(msg);
    exit(1);
}

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

inline static char* dup_cstr(const char* s) {
    size_t n = strlen(s) + 1;
    char* p = (char*)malloc(n);
    if (!p) die("oom");
    memcpy(p, s, n);
    return p;
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

// Casting
template <typename E>
inline constexpr std::uint8_t rccast(E e) {
    return static_cast<int>(e);
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

static constexpr int _ERR = 1;

inline Asura::Result<std::string, int> findPath(const fs::path& targetPath, const fs::path& startDir = fs::current_path()) {
    fs::path dir = startDir;

    while (true) {
        fs::path potentialPath = dir / targetPath;
        if (fs::exists(potentialPath)) {
            // return fs::absolute(potentialPath);
            return Asura::Result<std::string, int>(fs::absolute(potentialPath));
        }

        if (dir == dir.root_path()) {
            return Asura::Result<std::string, int>(_ERR);
        }

        dir = dir.parent_path();
    }
}

