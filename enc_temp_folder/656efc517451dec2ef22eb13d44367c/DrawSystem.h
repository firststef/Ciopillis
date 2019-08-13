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
            const auto transform = e->Get<TransformComponent>();
            const auto sprite = e->Get<SpriteComponent>();

            if (sprite.texture.id == 0)
                DrawRectangleRec(transform.position, e->Get<SpriteComponent>().color);
            else {
                auto nPatch = NPatchInfo{ Rectangle{ (float)0.01f, (float)0.01f, (float)sprite.texture.width - 0.01f, (float)sprite.texture.height - 0.01f },
                    0,0,sprite.texture.width,sprite.texture.height, NPT_9PATCH };

                DrawTextureNPatch(sprite.texture, nPatch , transform.position,
                    Vector2{ 0, 0 }, 0.0f, RAYWHITE);
            }
        }
        EndDrawing();
    }
};