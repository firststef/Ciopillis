#pragma once
#include "Component.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    std::string name;
    Texture2D texture {0};
    Rectangle sourceRec = { 0,0,0,0 };
    Color color = RED;

    SpriteComponent(std::string name, Texture2D tex, Color color, Rectangle source = {0,0,0,0}) 
    : name(std::move(name)), texture(tex), color(color)
    {
        if (source == Rectangle{0,0,0,0})
        {
            sourceRec = { 0,0, (float) texture.width, (float) texture.height };
        }
    }
};
