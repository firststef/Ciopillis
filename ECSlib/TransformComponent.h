#pragma once
#include <raylib.h>
#include "Component.h"

struct TransformComponent : IComponent
{
    Rectangle position = {0,0,100,100};
    int zIndex = -1;

    TransformComponent()
    {
    }
    
    TransformComponent(Rectangle rect, int zIndex = -1) : position(rect), zIndex(zIndex) {}

};