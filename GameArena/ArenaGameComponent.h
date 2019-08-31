#pragma once
#include "Component.h"

struct ArenaGameComponent : IComponent
{
    enum GameStates
    {
        INIT,
        RUNNING,
        ENDED
    }                               state = INIT;

    std::vector<EntityPtr>          generatedEntities;

    EntityPtr player;
    EntityPtr enemy;
};
