#pragma once
#include <bitset>
#include <array>
#include "Event.h"

class ECSManager
{
public:
    Pool* pool = nullptr;
    SystemManager* systemManager = nullptr;
    EventManager* eventManager = nullptr;

    ECSManager()
    {
        pool = new Pool;
        eventManager = new EventManager;
        systemManager = new SystemManager;

        systemManager->SetDependencies(pool, nullptr, eventManager);
    }

    void Initialize() const
    {
        systemManager->Initialize();
    }

    void Update() const 
    {
        systemManager->Execute();
    }

    ~ECSManager()
    {
        delete systemManager;
        delete eventManager;
        delete pool;
    }
};
