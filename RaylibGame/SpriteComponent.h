#pragma once
#include "Component.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    std::string name;

    Color color = RED;
    Texture *texture = nullptr;

    SpriteComponent(std::string name, Color color) : name(std::move(name)), color(color) {}
};
