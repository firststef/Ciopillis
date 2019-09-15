#pragma once
struct KeyboardEvent : IEvent
{
    EntityPtr entity;

    std::vector<int> pressedKeys;
    std::vector<int> releasedKeys;
    std::vector<int> heldKeys;

    KeyboardEvent(EntityPtr entity, std::vector<int> pressedKeys, std::vector<int> releasedKeys, std::vector<int> heldKeys)
        : entity(entity), pressedKeys(std::move(pressedKeys)), releasedKeys(std::move(releasedKeys)), heldKeys(std::move(heldKeys))
    {}
};