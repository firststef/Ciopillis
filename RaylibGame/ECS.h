#pragma once
#include <bitset>
#include <array>
#include "Event.h"

class ECSManager
{
public:
    Pool pool;
    SystemManager systemManager;
    EventManager eventManager;

    ECSManager()
    {
        systemManager.SetDependencies(&pool, nullptr, &eventManager);
    }

    void Initialize() 
    {
        systemManager.Initialize();
    }

    void Update()  
    {
        systemManager.Execute();
    }

    ~ECSManager()
    {
    }
};
