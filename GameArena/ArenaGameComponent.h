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

    enum CurrentAction
    {
        IDLE,
        MOVE,
        ATTACK_X,
        ATTACK_Z
    };

    EntityPtr player;
    CurrentAction currentActionPlayer = IDLE;
    Vector2 lastAxesPlayer {-1,-1};
    std::shared_ptr<bool> playerOrientation = std::make_shared<bool>();

    bool blockPlayerInput = false;

    EntityPtr enemy;
    Vector2 lastAxesEnemy{ -1,-1 };
    CurrentAction currentActionEnemy = IDLE;
};
