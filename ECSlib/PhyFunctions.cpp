#include "PhyFunctions2.h"
#define _STDBOOL_H
#define PHYSAC_IMPLEMENTATION
#define PHYSAC_STANDALONE
#define PHYSAC_NO_THREADS
#include <physac.h>

enum Type
{
    CIRCLE,
    RECTANGLE,
    POLYGON
};

PhysicsBody WIN_GetBody(int type, float x, float y, float first, float second, float third)
{
    PhysicsBody body;

    switch (type)
    {
    case CIRCLE:
        body = CreatePhysicsBodyCircle(Vector2{ x,y }, first, second);
        break;
    case RECTANGLE:
        body = CreatePhysicsBodyRectangle(Vector2{ x,y }, first, second, third);
        break;
    default:
    case POLYGON:
        body = CreatePhysicsBodyPolygon(Vector2{ x,y }, first, second, third);
        break;
    }

    return body;
}

void WIN_InitPhysics()
{
    InitPhysics();
}

void WIN_SetPhysicsGravity(float x, float y)
{
    SetPhysicsGravity(x, y);
}

void WIN_RunPhysicsStep()
{
    RunPhysicsStep();
}

void WIN_ClosePhysics()
{
    ClosePhysics();
}

#undef _STDBOOL_H
#undef PHYSAC_STANDALONE
#undef PHYSAC_IMPLEMENTATION