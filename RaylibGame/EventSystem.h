#pragma once
#include "Components.h"
#include "System.h"
#include "GridContainerSystem.h"

class EventSystem : public ISystem
{
public:

    void Initialize() override
    {
    }

    void Execute() override
    {
    }

    void Receive(const MouseEvent& event)//probabil aici apeleaza game server
    {
        if (event.type == MouseEvent::MOUSE_PRESS)
        {
            std::printf("Press\n");
        } 
        else if(event.type == MouseEvent::MOUSE_BEGIN_DRAG)
        {
            std::printf("Begin\n");

            if (event.entity->Has(1 << GetTypeID<GridContainerChildComponent>())) {
                auto parent = event.entity->Get<GridContainerChildComponent>().parent;
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::REMOVE, event.entity, parent);
            }
        }
        else if (event.type == MouseEvent::MOUSE_CONTINUE_DRAG) 
        {
            std::printf("Continue\n");
        }
        else if (event.type == MouseEvent::MOUSE_END_DRAG) 
        {
            std::printf("End\n");

            const auto coord = event.entity->Get<TransformComponent>().position;
            Vector2 center = { coord.x + coord.width / 2, coord.y + coord.height / 2 };
;
            auto objects = pool->GetEntities(1 << GetTypeID<TransformComponent>());
            std::sort(objects.begin(), objects.end(), [](EntityPtr left, EntityPtr right)->bool
            {
                return left->Get<TransformComponent>().zIndex > right->Get<TransformComponent>().zIndex;
            });

            EntityPtr parent;
            for (auto& zObj : objects)
            {
                if (zObj->Has(1 << GetTypeID<GridContainerComponent>()))
                {
                    const auto parentCoord = zObj->Get<TransformComponent>().position;

                    if (CheckCollisionPointRec(center, parentCoord))
                    {
                        parent = zObj;
                        break;
                    }
                }
            }

            if (parent != nullptr)
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, parent);
        }
    }

    bool QueryServer()
    {
        
    }
};
