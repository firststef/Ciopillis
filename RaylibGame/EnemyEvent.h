#pragma once
#include "Event.h"
#include "EventComponent.h"

struct EnemyEvent : IEvent
{
    enum EventAction
    {
        ENEMY_TURN
    } action;

    EnemyEvent(EventAction action) : action(action)
    {
    }
};
