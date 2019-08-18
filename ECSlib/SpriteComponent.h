#pragma once
#include "Component.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    std::string name;
    Texture2D texture {0};
    Color color = RED;

    SpriteComponent(std::string name, Texture2D tex, Color color) : name(std::move(name)), texture(tex), color(color) {}
};
