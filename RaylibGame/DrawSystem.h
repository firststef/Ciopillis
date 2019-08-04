#pragma once
#include "raylib.h"
#include "Components.h"
#include "System.h"

class DrawSystem : public ISystem
{
public:
    void Initialize() override {}

    void Execute() override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto& e : pool->GetEntities(1<<GetTypeID<TransformComponent>() | 1<<GetTypeID<SpriteComponent>()))
        {
            DrawRectangleRec(e->Get<TransformComponent>().position, e->Get<SpriteComponent>().color);
        }
        EndDrawing();
    }
};