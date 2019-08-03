#pragma once
#include "Component.h"

struct TransformComponent : IComponent
{
    Rectangle rectangle;

    TransformComponent()
    {
        rectangle.x = 0;
        rectangle.y = 0;
        rectangle.width = 100;
        rectangle.height = 100;
    }
    
    TransformComponent(Rectangle rect) : rectangle(rect) {}

};