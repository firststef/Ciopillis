#pragma once
#include "Event.h"
#include "TextureManager.h"

class ECSManager
{
public:
    TextureManager textureManager;
    Pool pool;
    SystemManager systemManager;
    EventManager eventManager;

    ECSManager()
    {
        systemManager.SetDependencies(&pool, &textureManager, nullptr, &eventManager);
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
