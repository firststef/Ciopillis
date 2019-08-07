#pragma once
#include "Component.h"

class Entity
{
    bool active = true;
    std::vector<std::shared_ptr<IComponent>> components;

    ComponentArray componentArray;
    ComponentBitset componentBitset;
public:

    Entity() = default;
    
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
        return componentBitset[GetTypeID<T>()];
    }
    bool Has(const ComponentBitset& bitset) const
    {
        return ((bitset & componentBitset)==bitset);
    }

    template<typename T, typename... TArgs>
    T& Add(TArgs&&... mArgs)
    {
        T* c(new T(std::forward<TArgs>(mArgs)...));
        c->entity = this;
        std::shared_ptr<IComponent> uPtr{ c };
        components.push_back(uPtr);

        componentArray[GetTypeID<T>()] = c;
        componentBitset[GetTypeID<T>()] = true;

        c->Init();
        return *c;
    }

    template<typename T>
    T& Get() const
    {
        auto ptr = componentArray[GetTypeID<T>()];
        return *static_cast<T*>(ptr);
    }

    template <typename T, typename... TArgs>
    T& Replace(TArgs&&... mArgs)
    {
        componentArray[GetTypeID<T>()] = { std::forward<TArgs>(mArgs)... };
        return componentArray[GetTypeID<T>()];
    }

    template <typename T>
    void Remove()
    {
        int idx = 0;
        for (auto& ptr : components)
        {
            if (ptr.get() == componentArray[GetTypeID<T>()])
            {
                components.erase(components.begin() + idx);
                break;
            }
            ++idx;
        }
        componentArray[GetTypeID<T>()] = nullptr;
        componentBitset[GetTypeID<T>()] = 0;
    }
};

typedef std::shared_ptr<Entity> EntityPtr;