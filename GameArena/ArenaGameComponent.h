#pragma once
#include "Component.h"

struct ArenaGameComponent : IComponent
{
    enum GameStates
    {
        INIT,
        RUNNING,
        ENDED
    }
    state = INIT;
};
