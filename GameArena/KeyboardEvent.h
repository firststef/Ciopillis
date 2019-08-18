#pragma once
struct KeyboardEvent : IEvent
{
    enum KeyboardAction
    {
        NONE,
        PRESSED,
        HOLD,
        RELEASED
    } action;

    int key;

    KeyboardEvent(KeyboardAction action, int key) : action(action), key(key) {}
};