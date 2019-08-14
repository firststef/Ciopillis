#pragma once
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
