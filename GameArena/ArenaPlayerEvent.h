#pragma once
struct ArenaPlayerEvent : IEvent
{
    enum Action
    {
        MOVE,
        ATTACK_X,
        ATTACK_Y
    } action;

    Vector2 axes;

    ArenaPlayerEvent(Action action, Vector2 axes) : action(action), axes(axes) {}
};