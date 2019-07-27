#pragma once
#include <vector>
#include <bitset>
#include <array>

class Component;
class Entity;

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

constexpr std::size_t maxComponents = 32;

using ComponentBitset = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;

class Component
{
public:
    Entity* entity;

    virtual void Init() {}
    virtual void Update() {}
    virtual void Draw() {}

    virtual ~Component() {}
};

class Entity
{
private:
    bool active = true;
    std::vector<std::unique_ptr<Component>> components;

    ComponentArray componentArray;
    ComponentBitset componentBitset;
public:
    void Update()
    {
        for (auto& c : components) c->Update();
    }

    void Draw()
    {
        for (auto& c : components) c->Draw();
    }

    bool IsActive() const { return active; };
    void Destroy() { active = true; }

    template<typename T>
    bool HasComponent() const
    {
        return componentBitset[GetTypeID<T>()];
    }

    template<typename T, typename... TArgs>
    T& AddComponent(TArgs&&... mArgs)
    {
        T* c(new T(std::forward<TArgs>(mArgs)));
        c->entity = this;
        std::unique_ptr<Component> uPtr{ c };
        components.emplace_back(std::move(uPtr));

        componentArray[GetTypeID<T>()] = c;
        componentBitset[GetTypeID<T>()] = true;

        c->Init();
        return *c;
    }

    template<typename T>
    T& GetComponent() const
    {
        auto ptr(componentArray[GetTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }
};

class Manager
{
private:
    std::vector<std::unique_ptr<Entity>> entities;

public:
    void Update()
    {
        for (auto& e : entities) e->Update();
    }

    void Draw()
    {
        for (auto& e : entities) e->Draw();
    }

    void Refresh()
    {
        entities.erase(std::remove_if(std::begin(entities),std::end(entities),
            [](const std::unique_ptr<Entity> &mEntity)
        {
            return !mEntity->IsActive();
        }),
            std::end(entities));
    }

    Entity& AddEntity()
    {
        Entity* e = new Entity;
        std::unique_ptr<Entity> uPtr{ e };
        entities.emplace_back(std::move(uPtr));
        return *e;
    }
};