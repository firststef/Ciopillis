#pragma once
#include "Event.h"

struct MouseEvent : IEvent
{
    enum MouseAction
    {
        NONE,
        MOUSE_PRESS,
        MOUSE_BEGIN_DRAG,
        MOUSE_CONTINUE_DRAG,
        MOUSE_END_DRAG
    } type = NONE;

    MouseEvent(MouseAction action) : type(action)
    {
    }

    MouseEvent(char x)
    {

    }
};