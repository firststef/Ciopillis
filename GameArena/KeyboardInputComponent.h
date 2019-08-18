#pragma once
#include "Component.h"

struct KeyboardInputComponent : IComponent
{
    std::vector<int> gestures;

    KeyboardInputComponent(std::vector<int> g) : gestures(g)
    {
    }
};