#pragma once
#include "TextureManager.h"
#include "EventManager.h"

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

    void Destroy()
    {
        systemManager.Destroy();
    }

    ~ECSManager()
    {
    }
};
