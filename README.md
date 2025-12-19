# Asura Engine

<table>
  <tr>
    <td>
      <img src="imgs/team_sigma.png" alt="Team Sigma" width="240">
    </td>
    <td>
      A <strong>Team Sigma</strong> Project
    </td>
  </tr>
</table>


Asura is a C++ engine/framework for game development. It’s currently being used in two WIP projects, and I plan to open-source them once completed.

Asura does not handle windowing and context creation. Instead, it leans on [sokol](https://github.com/floooh/sokol) for cross-platform graphics (Metal on macOS, D3D11 on Windows, and OpenGL on Linux), making it effectively just a renderer/wrapper over sokol.

This is similar to another one of my projects, [SmallGraphicsLayer](https://github.com/shreejitmurthy/SmallGraphicsLayer). SGL is on indefinite pause (commits are scant), but it is a useful reference for Asura since it uses sokol and contains some design principles that reflect in Asura.

## Dependencies
Asura requires a few dependencies:
- [sokol](https://github.com/floooh/sokol): Cross-platform graphics abstraction.
- [spdlog](https://github.com/gabime/spdlog): Fast console logging.
- [json](https://github.com/nlohmann/json): Used in asset metadata creation for faster game reloads.

## Features
### Basic Debug Text Rendering
```cpp
#include <asura/asura.h>

void draw() {
    Asura::begin(pass_action);

    Asura::Debug::print("Hello, World!", {red, green, blue});
    Asura::Debug::print(std::format("FPS: {}", std::round(1 / delta_time)));

    Asura::end();
}
```
### "Registry" Based Asset Loading
```cpp
#include <asura/asura.h>

Asura::SpriteRenderer sr;
Asura::FontRenderer fr;

enum class SpriteID : uint8_t {
    Player = 1,
    Enemy
};

static const std::vector<Asura::ResourceDef> spriteRegistry = {
    // string names must match the filename: player.png, enemy.png, ...
    { "player", (int)SpriteID::Player },
    { "enemy",  (int)SpriteID::Enemy },
};

enum class FontID : uint8_t {
    Alagard = 1,
};

static const std::vector<Asura::ResourceDef> fontRegistry = {
    // string name, ID, font size
    {"alagard", (int)FontID::Alagard, 16}
};

void init() {
    Asura::init();

    sr.init("res/images/", spriteRegistry);
    
    fr.init("res/fonts/", fontRegistry);
}
```
### GPU Instanced Sprite Renderer
Arguments for the `push()` function are: `E id, vec2 position, vec2 scale = {1, 1}, float rotation = 0.f, sg_color tint = sg_white, vec2 pivot = Asura::Pivot::TopLeft(), vec2 pivot_px = {0, 0}`
```cpp
#include <asura/asura.h>

void draw() {
    Asura::begin(pass_action);

    sr.push(SpriteID::Player, {100, 100});

    sr.push(SpriteID::Enemy,  {400, 500}, {2, 2}, program_runtime, Asura::Pivot::Centre());

    sr.render();

    Asura::end();
}
```
### Bitmap Font Rendering
Arguments for the `queue()` function are: `E id, std::string_view text, glm::vec2 pos, float scale = 1.f, sg_color tint = sg_white`
```cpp
#include <asura/asura.h>

void draw() {
    Asura::begin(pass_action);

    fr.queue(FontID::Alagard, "very epic code", {250, 250}, 2);

    fr.render();

    Asura::end();
}
```
### Informative Logging
<p align="center">
    <img src="imgs/logging.png" width=600>
</p>

### Basic Math Functions
Asura's math headers contain only what's necessary for Asura to run, no bloat.
```cpp
#include "vec2.hh"
#include "vec3.hh"
#include "vec4.hh"
#include "mat4.hh"
using namespace Asura::Math;

Vec2 a{1,2}, b{4,6};                      // init + basic ops
Vec2 dir = (b - a).normalized(), n2 = dir.perp();  float dist2 = (b - a).length2();
Vec2 eased = Vec2::lerp(a, b, 0.25f);     // nice for animation / smoothing

iVec2 A{0,0}, B{1920,1080};               // integer grid -> float interpolation
Vec2 px = Vec2::lerp(A, B, 0.5f);

Vec3 up{0,1,0}, fwd{0,0,-1};              // cross gives a perpendicular axis
Vec3 right = up.cross(fwd).normalized();

Mat4 M = Mat4::translate({2,0,0}) * Mat4::rotateY(0.5f) * Mat4::scale({2,1,2});
Vec3 worldP = M.multiplyPoint({1,2,3});   // point gets translation, a vector wouldn’t

Mat4 P = Mat4::orthoCentered(800, 600, 0.1f, 100.f, true);
float yScale = P(1,1);                    // direct matrix element access (row, col)

Vec4 clip{worldP.x, worldP.y, worldP.z, 1.f} * 0.5f;  // handy homogeneous packaging
```

## TODO
- [x] Use custom math header
- [x] Safe directory parsing.
- [ ] Primitives (basic lines and shapes) for debugging
- [ ] ImGui support
- [ ] Async asset loading

