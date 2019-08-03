#pragma once
#include <vector>
#include "System.h"
#include <functional>

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

struct IEvent
{
};

template<typename Event>
struct Receiver
{
    virtual void Receive(const Event& event) {}
};

struct ReceiverCallbacksHolder
{
    std::vector<std::function<void(const void*)>> typeSubscribers;
};

template<typename E>
struct TFuncToVoidFuncConverter
{
    TFuncToVoidFuncConverter(std::function<void(const E &)> callback) : callback(callback) {}
    void operator()(const void *event) { callback(*(static_cast<const E*>(event))); }
    std::function<void(const E &)> callback;
};

struct EventManager
{
    std::array<ReceiverCallbacksHolder*, 32> holders {nullptr};

    template<typename Event, typename System>
    void Subscribe(const std::shared_ptr<System>& system)
    {
        void (System::*Receive)(const Event &) = &System::Receive;
        if (Receive == nullptr)
            throw std::exception("Function conversion failed");

        auto converter = TFuncToVoidFuncConverter<Event>(std::bind(Receive, static_cast<System*>(system.get()), std::placeholders::_1));

        if (holders[GetEventTypeID<Event>()] == nullptr)
        {
            holders[GetEventTypeID<Event>()] = new ReceiverCallbacksHolder;
        }

        holders[GetEventTypeID<Event>()]->typeSubscribers.push_back(nullptr);
        auto& copy = holders[GetEventTypeID<Event>()]->typeSubscribers.back();
        copy = std::move(converter);
    }

    template<typename Event, typename System>
    void Unsubscribe(const std::shared_ptr<ISystem>& system)
    {
        void (System::*receive)(const Event &) = &System::receive;
        auto func = std::bind(receive, system.get(), std::placeholders::_1);

        int i = 0;
        for (auto& f : holders[GetEventTypeID<Event>()]->typeSubscribers)
        {
            if (f == func)
                holders[GetEventTypeID<Event>()]->typeSubscribers.erase(holders[GetEventTypeID<Event>()]->typeSubscribers.begin() + i);
            ++i;
        }
    }

    template<typename T, typename... TArgs>
    void Notify(TArgs&&... mArgs)
    {
        auto it = holders[GetEventTypeID<T>()]->typeSubscribers.begin();
        auto end = holders[GetEventTypeID<T>()]->typeSubscribers.end();
        for (auto func = *it; it != end; ++it)
        {
            T event = T(std::forward<TArgs>(mArgs)...);
            func(&event);
        }
    }
};