#pragma once
struct CardGameEnemyEvent : IEvent
{
    enum EventAction
    {
        ENEMY_TURN
    } action;

    CardGameEnemyEvent(EventAction action) : action(action)
    {
    }
};
