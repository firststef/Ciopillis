#pragma once
#include "raylib.h"
#include "Components.h"
#include "System.h"
#include "Event.h"

struct EventSystem : ISystem, Receiver<MouseEvent>
{
    void Initialize() override
    {
    }

    void Execute() override
    {
    }

    void Receive(const MouseEvent& event) override
    {
        switch (event.type)//probabil aici apeleaza game server
        {
        case MouseEvent::MOUSE_PRESS:
            std::printf("Press\n");
            break;
        case MouseEvent::MOUSE_BEGIN_DRAG:
            std::printf("Begin\n");
            break;
        case MouseEvent::MOUSE_CONTINUE_DRAG:
            std::printf("Continue\n");
            break;
        case MouseEvent::MOUSE_END_DRAG:
            std::printf("End\n"); //---> probabil referinta catre sistem grid si facut un request catre el pentru update
            break;
        default:
            break;
        }
    }
};