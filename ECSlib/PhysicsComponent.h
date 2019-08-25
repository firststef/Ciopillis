#pragma once
#include "Component.h"
#include <raylib.h>
#include <physac.h>
#include "PhyFunctions.h"

struct PhysicsComponent : IComponent
{
    PhysicsBody body;

    enum Type
    {
        CIRCLE,
        RECTANGLE,
        POLYGON
    }type;   

    PhysicsComponent(Type type, float x, float y, float first, float second, float third) : type(type)
    {
        body = WIN_GetBody((int) type, x, y, first, second, third);
    }
};
