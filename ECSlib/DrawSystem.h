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
                DrawTexturePro(sprite.texture, sprite.sourceRec, transform.position,
                    Vector2{ 0, 0 }, 0.0f, RAYWHITE);
            }
        }

#if defined(_DEBUG) && defined(ARENA)

        int bodiesCount = GetPhysicsBodiesCount();
        for (int i = 0; i < bodiesCount; i++)
        {
            PhysicsBody body = GetPhysicsBody(i);

            if (body != NULL)
            {
                int vertexCount = GetPhysicsShapeVerticesCount(i);
                for (int j = 0; j < vertexCount; j++)
                {
                    // Get physics bodies shape vertices to draw lines
                    Vector2 vertexA = GetPhysicsShapeVertex(body, j);

                    int jj = (((j + 1) < vertexCount) ? (j + 1) : 0);   // Get next vertex or first to close the shape
                    Vector2 vertexB = GetPhysicsShapeVertex(body, jj);

                    DrawLineV(vertexA, vertexB, GREEN);     // Draw a line between two vertex positions
                }
            }
        }

#endif

        EndDrawing();
    }
};