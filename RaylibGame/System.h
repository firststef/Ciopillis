#pragma once
#include "Pool.h"

struct EventManager;
struct IEvent;

struct ISystem
{
    Pool* pool = nullptr;
    ISystem* systemManager = nullptr; 
    EventManager* eventManager = nullptr;

    ISystem() {}

    virtual void SetDependencies(Pool* pool, ISystem* systemManager, EventManager* eventManager)
    {
        this->pool = pool;
        this->systemManager = systemManager;
        this->eventManager = eventManager;
    }
    virtual void Initialize() {}
    virtual void Execute() {}
};

struct SystemManager : public ISystem
{
    std::vector<std::shared_ptr<ISystem>> systems;

    void Initialize() override
    {
        for (const auto& ptr : systems )
        {
            ptr->Initialize();
        }
    }

    void Execute() override
    {
        for (const auto& ptr : systems)
        {
            ptr->Execute();
        }
    }

    void AddSystem(std::shared_ptr<ISystem> ptr)
    {
        systems.push_back(std::dynamic_pointer_cast<ISystem>(ptr));

        ptr->SetDependencies(pool, this, eventManager);
    }
};