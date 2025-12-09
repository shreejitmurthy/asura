# Asura Engine

Asura is a C++ engine/framework for game development. It’s currently being used in two WIP projects, and I plan to open-source it once completed.

Asura does not handle windowing and context creation. Instead, it leans on [sokol](https://github.com/floooh/sokol) for cross-platform graphics (Metal on macOS, D3D11 on Windows, and OpenGL on Linux), making it effectively just a renderer.

This is similar to another one of my projects, [SmallGraphicsLayer](https://github.com/shreejitmurthy/SmallGraphicsLayer). SGL is on indefinite pause (commits are scant), but it is a useful reference for Asura since it uses sokol and contains some design principles that reflect in Asura.

Asura now fully relies on sokol, and cannot interop with any other windowing and context creation library (glfw, SDL, etc.). Right now the sokol windowing layer is used extremely minimally (by design). If I choose to create a game using anything other than `sokol_app`, then I will make the appropriate changes to support this.

## Features
### Basic Debug Text Rendering
```cpp
void draw() {
    Asura::begin(pass_action);

    Asura::debug("Hello, World!", {red, green, blue});
    Asura::debug(std::format("FPS: {}", std::round(1 / delta_time)));

    Asura::end();
}
```
### "Registry" Based Asset Loading
```cpp
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
    {"alagard", rccast(FontID::Alagard), 16}
};

void init() {
    Asura::init();

    sr.init("../res/images/", spriteRegistry);
    
    fr.init("../res/fonts/", fontRegistry);
}
```
### GPU Instanced Sprite Renderer
Arguments for the `push()` function are: `E id, vec2 position, vec2 scale = {1, 1}, float rotation = 0.f, sg_color tint = sg_white, vec2 pivot = Asura::Pivot::TopLeft(), vec2 pivot_px = {0, 0}`
```cpp
void draw() {
    Asura::begin(pass_action);

    sr.push(SpriteID::Player, {100, 100});
    sr.push(SpriteID::Enemy,  {400, 500}, {2, 2}, program_runtime, Asura::Pivot::Centre());  // position, scale, rotation

    sr.render();

    Asura::end();
}
```
### Bitmap Font Rendering
Arguments for the `queue()` function are: `E id, std::string_view text, glm::vec2 pos, float scale = 1.f, sg_color tint = sg_white`
```cpp
void draw() {
    Asura::begin(pass_action);

    fr.queue(FontID::Alagard, "very epic code", {250, 250}, 2);

    fr.render();

    Asura::end();
}
```
### Informative Logging
<p align="center">
    <img src="scs/logging.png" width = 600>
</p>