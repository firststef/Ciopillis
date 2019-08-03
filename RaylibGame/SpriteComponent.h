#pragma once
#include "Component.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    Color color = RED;
    Texture *texture = nullptr;
};
