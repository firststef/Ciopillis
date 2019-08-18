#pragma once

struct SystemControlEvent : IEvent
{
    enum ControlAction
    {
        NONE,
        ENABLE,
        DISABLE
    } controlAction = NONE;

    std::string systemName;

    SystemControlEvent(ControlAction action, std::string name) : controlAction(action), systemName(name)
    {
        
    }
};
