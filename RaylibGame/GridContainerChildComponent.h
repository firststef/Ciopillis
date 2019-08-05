#pragma once
#include "Component.h"

struct GridContainerChildComponent : IComponent
{
    EntityPtr parent;

    GridContainerChildComponent(EntityPtr p) : parent(p) {}
};
