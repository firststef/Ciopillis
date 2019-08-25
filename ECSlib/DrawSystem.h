#pragma once
#include "raylib.h"
#include "Components.h"
#include "System.h"

class DrawSystem : public ISystem
{
public:
    DrawSystem() : ISystem(std::string("DrawSystem")) {}

    void Initialize() override {}

    void Execute() override
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        auto entities = pool->GetEntities(1 << GetTypeID<TransformComponent>() | 1 << GetTypeID<SpriteComponent>());
        std::sort(entities.begin(), entities.end(), [](EntityPtr a, EntityPtr b)
        {
            return a->Get<TransformComponent>().zIndex < b->Get<TransformComponent>().zIndex;
        });

        for (auto& e : entities)
        {
            const auto transform = e->Get<TransformComponent>();
            const auto sprite = e->Get<SpriteComponent>();

            if (sprite.texture.id == 0)
                DrawRectangleRec(transform.position, e->Get<SpriteComponent>().color);
            else {
                //TODO: Jittering is caused by unscaled resize
                DrawTexturePro(sprite.texture, { 0,0,(float)sprite.texture.width, (float)sprite.texture.height }, transform.position,
                    Vector2{ 0, 0 }, 0.0f, RAYWHITE);
            }
        }
        EndDrawing();
    }
};