#pragma once
struct KeyboardEvent : IEvent
{
    std::vector<int> pressedKeys;
    std::vector<int> releasedKeys;
    std::vector<int> heldKeys;

    KeyboardEvent(std::vector<int> pressedKeys, std::vector<int> releasedKeys, std::vector<int> heldKeys)
        : pressedKeys(std::move(pressedKeys)), releasedKeys(std::move(releasedKeys)), heldKeys(std::move(heldKeys))
    {}
};