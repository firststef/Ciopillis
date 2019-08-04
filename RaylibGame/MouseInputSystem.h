#pragma once
#include "raylib.h"
#include "System.h"
#include "Components.h"

class MouseInputSystem : public ISystem
{
    EntityPtr entity;
    Vector2 mouseGrab = {0,0};
    bool dragStarted = false;
    int previousGesture = GESTURE_NONE;

    void OnSelect(Vector2 mouse)
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<MouseInputComponent>() | 1 << GetTypeID<TransformComponent>()))
        {
            if (CheckCollisionPointRec(mouse, e->Get<TransformComponent>().position))
            {
                entity = e;
                break;//trebuie facut un sistem de selectie dupa z
            }
        }
    }

    void OnPress(Vector2 mouse)
    {
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_PRESS);
    }

    void OnBeginDrag(Vector2 mouse)
    {
        dragStarted = true;
        auto r = entity->Get<TransformComponent>().position;
        mouseGrab = { mouse.x - r.x, mouse.y - r.y };
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_BEGIN_DRAG);
    }

    void OnContinueDrag(Vector2 mouse) 
    {
        auto r = entity->Get<TransformComponent>().position;
        entity->Get<TransformComponent>().position = { mouse.x - mouseGrab.x,mouse.y - mouseGrab.y, r.width, r.height };
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_CONTINUE_DRAG);
    }

    void OnEndDrag()
    {
        dragStarted = false;
        mouseGrab = { 0,0 };
        eventManager->Notify<MouseEvent>(MouseEvent::MOUSE_END_DRAG);
        entity = nullptr;
    }

public:

    void Initialize() override {}
    void Execute() override
    {
        const int lastGesture = GetGestureDetected();
        const Vector2 mouse = GetMousePosition();

        if (lastGesture == GESTURE_NONE)
        {
            //this could be the place for uninit
        }

        if (lastGesture == GESTURE_TAP)
        {
            OnSelect(mouse);
        } 
        else if( (previousGesture == GESTURE_TAP && lastGesture == GESTURE_NONE ) || (previousGesture == GESTURE_HOLD && lastGesture == GESTURE_NONE) || (previousGesture == GESTURE_DOUBLETAP && lastGesture == GESTURE_NONE))
        {
            if (entity != nullptr)
                OnPress(mouse);
        }

        if (lastGesture == GESTURE_DRAG)
        {
            if (!dragStarted) {

                if (entity != nullptr /*&& lastSelectedObject->GetIsSelectable()*/) {
                    OnBeginDrag(mouse);
                }
            }
            else
            {
                OnContinueDrag(mouse);
            }
        }
        else if (previousGesture == GESTURE_DRAG) {
            if (dragStarted)
                OnEndDrag();
        }

        previousGesture = lastGesture;
    }
};
