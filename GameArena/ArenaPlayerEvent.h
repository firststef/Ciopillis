#pragma once
struct ArenaPlayerEvent : IEvent
{
	EntityPtr arena;
	
    enum Action
    {
        MOVE,
        ATTACK_X,
        ATTACK_Z
    } action;

    Vector2 axes;

    ArenaPlayerEvent(EntityPtr arena, Action action, Vector2 axes) : arena(arena), action(action), axes(axes) {}
};