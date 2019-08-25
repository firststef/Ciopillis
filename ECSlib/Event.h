#pragma once
#include <utility>
#include <vector>
#include <functional>

struct IEvent
{
};

using EventID = std::size_t;

struct BaseEvent {

    inline static EventID totalEventTypes = 0;

    inline static EventID GetEventID()
    {
        return totalEventTypes++;
    }
};

template <typename T>
inline EventID GetEventTypeID() noexcept
{
    static_assert(std::is_base_of<IEvent, T>::value, "T is not derived from event");

    static EventID typeID = BaseEvent::GetEventID();
    return typeID;
}