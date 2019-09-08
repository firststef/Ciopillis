#pragma once

struct DefferEvent : IEvent
{
    unsigned delayTime;
    std::function<void(void*)> func;
    void* context;

    DefferEvent(unsigned delayTime, std::function<void(void*)> func, void* context)
        : delayTime(delayTime), func(func), context(context)
    {}
};