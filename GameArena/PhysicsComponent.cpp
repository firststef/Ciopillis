#include "PhysicsComponent.h"

template<typename... TArgs>
PhysicsEntity(Type type, TArgs&&... mArgs)
{
    switch (type)
    {
    case CIRCLE:
        body = CreatePhysicsBodyCircle(mArgs...);
        break;
    case POLYGON:
        body = CreatePhysicsBodyPolygon(mArgs...);
        break;
    default:
    case RECTANGLE:
        body = CreatePhysicsBodyRectangle(mArgs...);
        break;
    }
    body->freezeOrient = true;
}