#pragma once

class ArenaSystem : public ISystem
{
    ArenaSystem() : ISystem(std::string("ArenaSystem")) {}

    void Initialize() override
    {   
    }

    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
        {

        }
    }
};