#pragma once
#include "raylib.h"
#include <vector>
#include <bitset>
#include <array>
#include "Component.h"
#include "Entity.h"
#include "System.h"

class ECSManager
{
public:
    Pool* pool;
    SystemManager* systemManager;

    ECSManager()
    {
        pool = new Pool;
        systemManager = new SystemManager(pool);
    }

    void Update() const 
    {
        systemManager->Execute();
    }

    ~ECSManager()
    {
        delete systemManager;
        delete pool;
    }
};
