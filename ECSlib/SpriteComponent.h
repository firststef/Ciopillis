#pragma once
#include "Component.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    std::string name;
    Texture2D texture {0};
    Color color = RED;

    SpriteComponent(std::string name, Color color) : name(std::move(name)), color(color) {}
    SpriteComponent(std::string name, Texture2D tex) :name(std::move(name)), texture(tex) {}//TODO: de unificat astea 2 ca sa poata face hue coloring by default
};
