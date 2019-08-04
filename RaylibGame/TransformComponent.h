#pragma once
#include "Component.h"

struct TransformComponent : IComponent
{
    Rectangle position;

    TransformComponent()
    {
        position.x = 0;
        position.y = 0;
        position.width = 100;
        position.height = 100;
    }
    
    TransformComponent(Rectangle rect) : position(rect) {}

};