#pragma once
#include "Pool.h"

class ISystem
{
protected:
    Pool* pool = nullptr;

public:

    ISystem() {}
    ISystem(Pool* pool) : pool(pool) {}

    virtual void SetPool(Pool* pool)
    {
        this->pool = pool;
    }
    virtual void Initialize() {};
    virtual void Execute() {};
};

class SystemManager : public ISystem
{
    std::vector<std::shared_ptr<ISystem>> systems;

public:

    SystemManager(Pool* pool) : ISystem(pool)
    {
        
    }

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

        ptr->SetPool(pool);
    }
};