#pragma once
#include "HitBoxEvent.h"

class HitBoxSystem : ISystem
{
public:
    HitBoxSystem() : ISystem("HitBoxSystem") {}

    void Initialize() override {}

    void Execute() override
    {
        std::vector<HitBoxComponent*> containers;

        auto entities = pool->GetEntities(1 << GetTypeID<HitBoxComponent>());

        for (auto e : entities)
        {
            containers.push_back(&e->Get<HitBoxComponent>());
        }

        for (unsigned i = 0; i < containers.size() - 1; ++i)
            for (unsigned j = 1; j < containers.size(); ++j)
            {
                for (auto& holder_left : containers[i]->cont.holders)
                {
                    for (auto& holder_right : containers[j]->cont.holders)
                    {
                        if (holder_left.shape.type == Shape::RECTANGLE == holder_right.shape.type)
                        {
                            if (CheckCollisionRecs(holder_left.shape.rectangle.rec, holder_right.shape.rectangle.rec))
                            {
                                eventManager->Notify<HitBoxEvent>();
                            }
                        }
                        //TODO: cercuri, etc
                    }
                }
            }
    }
};
