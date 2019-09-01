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
    Vector2 lastAxesPlayer {-1,-1};

    EntityPtr enemy;
    Vector2 lastAxesEnemy{ -1,-1 };
};
