#pragma once
#include "HitBoxEvent.h"

class HitBoxSystem : public ISystem
{
public:
    HitBoxSystem() : ISystem("HitBoxSystem") {}

    void Initialize() override {}

    void Execute() override
    {
        std::vector<HitBoxComponent*> containers;

        auto entities = pool->GetEntities(1 << GetTypeID<HitBoxComponent>());

        containers.reserve(entities.size());
        for (auto e : entities)
        {
            auto& box = e->Get<HitBoxComponent>();
            box.cont.Update();

            containers.push_back(&e->Get<HitBoxComponent>());
        }

        std::vector<HitBoxTriggerInfo> allTriggerInfos;

        for (unsigned i = 0; i < containers.size() - 1; ++i) {
            for (unsigned j = 1; j < containers.size(); ++j)
            {
                for (auto& holder_left : containers[i]->cont.holders)
                {
                    for (auto& holder_right : containers[j]->cont.holders)
                    {
                        if (holder_left.shape.objectClass == holder_right.shape.objectClass)
                        {
                            continue;
                        }

                        if (holder_left.shape.type == Shape::RECTANGLE && holder_right.shape.type == Shape::RECTANGLE)
                        {
                            if (CheckCollisionRecs(holder_left.shape.rectangle.rec, holder_right.shape.rectangle.rec))
                            {
                                allTriggerInfos.push_back(HitBoxTriggerInfo{ holder_left.shape.id, holder_right.shape.id });
                            }
                        }
                        else if (holder_left.shape.type == Shape::CIRCLE && holder_right.shape.type == Shape::CIRCLE)
                        {
                            if (CheckCollisionCircles(Vector2{ float(holder_left.shape.circle.x), float(holder_left.shape.circle.x) }, holder_left.shape.circle.radius,
                                Vector2{ float(holder_right.shape.circle.x), float(holder_right.shape.circle.x) }, holder_right.shape.circle.radius))
                            {
                                allTriggerInfos.push_back(HitBoxTriggerInfo{ holder_left.shape.id, holder_right.shape.id });
                            }
                        }
                        else if (holder_left.shape.type == Shape::CIRCLE && Shape::RECTANGLE == holder_right.shape.type)
                        {
                            if (CheckCollisionCircleRec(Vector2{ float(holder_left.shape.circle.x), float(holder_left.shape.circle.x) }, holder_left.shape.circle.radius,
                                holder_right.shape.rectangle.rec))
                            {
                                allTriggerInfos.push_back(HitBoxTriggerInfo{ holder_left.shape.id, holder_right.shape.id });
                            }
                        }
                        else if (Shape::CIRCLE == holder_right.shape.type && holder_left.shape.type == Shape::RECTANGLE)
                        {
                            if (CheckCollisionCircleRec(Vector2{ float(holder_right.shape.circle.x), float(holder_right.shape.circle.x) }, holder_right.shape.circle.radius,
                                holder_left.shape.rectangle.rec))
                            {
                                allTriggerInfos.push_back(HitBoxTriggerInfo{ holder_left.shape.id, holder_right.shape.id });
                            }
                        }
                    }
                }
            }
        }

        if (! allTriggerInfos.empty())
            eventManager->Notify<HitBoxEvent>(allTriggerInfos);
    }
};
