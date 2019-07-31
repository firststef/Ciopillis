#pragma once
#include "Components.h"

struct EventComponent : IComponent
{
    enum EventType
    {
        NONE,
        MOUSE_PRESS,
        MOUSE_BEGIN_DRAG,
        MOUSE_CONTINUE_DRAG,
        MOUSE_END_DRAG
    } type = NONE;

    EventComponent() = default;
    EventComponent(EventType ev) : type(ev) {}
};