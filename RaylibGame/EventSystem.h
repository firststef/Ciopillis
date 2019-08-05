#pragma once
#include "Components.h"
#include "System.h"
#include "GridContainerSystem.h"

class EventSystem : public ISystem
{
public:
    EntityPtr parent;//trebuie scos si extras la end_Drag, asta e doar pt teste

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

            //parent = event.entity->Get<GridContainerChildComponent>().parent;
            //eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, parent);

            if (event.entity->Has(1 << GetTypeID<GridContainerChildComponent>()))
            {
                parent = event.entity->Get<GridContainerChildComponent>().parent;
                systemManager->Get<GridContainerSystem>()->ReleaseItem(parent, event.entity);
            }
        }
        else if (event.type == MouseEvent::MOUSE_CONTINUE_DRAG) 
        {
            std::printf("Continue\n");
        }
        else if (event.type == MouseEvent::MOUSE_END_DRAG) 
        {
            std::printf("End\n");
            
            //eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::REMOVE, event.entity, parent);
            systemManager->Get<GridContainerSystem>()->AddItem(parent, event.entity);
            parent = nullptr;
        }
    }
};
