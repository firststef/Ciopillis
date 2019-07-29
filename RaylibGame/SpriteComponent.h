#pragma once
#include "ECS.h"
#include "Components.h"
#include <raylib.h>

struct SpriteComponent : IComponent
{
    TransformComponent *position = nullptr;
    Color color = RED;
    Texture *texture = nullptr;

    SpriteComponent() = default;

    SpriteComponent(const char* path)
    {

    }

    void Init() override
    {
        position = &entity->Get<TransformComponent>();
    }
};
