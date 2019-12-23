#pragma once
#include "Component.h"
#include <vector>
#include <memory>

class Entity : public std::enable_shared_from_this<Entity>
{
    bool active = true;
    std::vector<ComponentPtr> components;

    ComponentArray componentArray = {};
    ComponentBitset componentBitset;
public:
    
    bool IsActive() const { return active; };
    void RemoveAllComponents()
    {
        components.clear();
    }
    void Destroy()
    {
        RemoveAllComponents();
        active = false;
    }

    template<typename T>
    bool Has() const
    {
        return componentBitset[GetComponentTypeID<T>()];
    }
    bool Has(const ComponentBitset& bitset) const
    {
        return ((bitset & componentBitset)==bitset);
    }

    template<typename T, typename... TArgs>
    T& Add(TArgs&&... mArgs)
    {
        T* c(new T(std::forward<TArgs>(mArgs)...));
        c->entity = shared_from_this();
        ComponentPtr uPtr{ c };
        components.push_back(uPtr);

        componentArray[GetComponentTypeID<T>()] = c;
        componentBitset[GetComponentTypeID<T>()] = true;

        c->Init();
        return *c;
    }

    template<typename T>
    T& Get() const
    {
        auto ptr = componentArray[GetComponentTypeID<T>()];
        return *static_cast<T*>(ptr);
    }

    template <typename T, typename... TArgs>
    T& Replace(TArgs&&... mArgs)
    {
        componentArray[GetComponentTypeID<T>()] = { std::forward<TArgs>(mArgs)... };
        return componentArray[GetComponentTypeID<T>()];
    }

    template <typename T>
    void Remove()
    {
        int idx = 0;
        for (auto& ptr : components)
        {
            if (ptr.get() == componentArray[GetComponentTypeID<T>()])
            {
                components.erase(components.begin() + idx);
                break;
            }
            ++idx;
        }
        componentArray[GetComponentTypeID<T>()] = nullptr;
        componentBitset[GetComponentTypeID<T>()] = 0;
    }
};

using EntityPtr = std::shared_ptr<Entity>;