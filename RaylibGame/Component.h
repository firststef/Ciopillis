#pragma once
#include <vector>
#include <bitset>
#include <array>

using ComponentID = std::size_t;

inline ComponentID GetComponentID()
{
    static ComponentID lastID = 0;
    return lastID++;
}

template <typename T>
inline ComponentID GetTypeID() noexcept
{
    static ComponentID typeID = GetComponentID();
    return typeID;
}

class Entity;

class IComponent
{
public:
    Entity* entity = nullptr;

    virtual void Init() {}

    virtual ~IComponent() {}
};

constexpr std::size_t maxComponents = 32;

using ComponentBitset = std::bitset<maxComponents>;
using ComponentArray = std::array<IComponent*, maxComponents>;