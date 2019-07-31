#pragma once
#include "raylib.h"
#include "Components.h"
#include "System.h"

class EventSystem : public ISystem
{
    void Initialize() override {}

    void Execute() override
    {
        auto ptr = pool->GetEntity(1 << GetTypeID<EventComponent>());
        if (ptr == nullptr)
            return;

        switch (ptr->Get<EventComponent>().type)//probabil aici apeleaza game server
        {
        case EventComponent::MOUSE_PRESS:
            std::printf("Press\n");
            break;
        case EventComponent::MOUSE_BEGIN_DRAG:
            std::printf("Begin\n");
            break;
        case EventComponent::MOUSE_CONTINUE_DRAG:
            std::printf("Continue\n");
            break;
        case EventComponent::MOUSE_END_DRAG:
            std::printf("End\n");
            break;
        default:
            break;
        }

        ptr->Remove<EventComponent>();
    }
};