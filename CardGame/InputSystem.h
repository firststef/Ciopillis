#pragma once
#include <raylib.h>
#include "System.h"
#include "InputComponent.h"

class InputSystem : public ISystem
{
    void Initialize() override
    {
        
    }

    void Execute() override
    {
        int lastGesture = GetGestureDetected();
        Vector2 mouse = GetMousePosition();

        for (auto e : pool->GetEntities(1 << GetTypeID<InputSystem>()))
        {

            if (lastGesture == GESTURE_NONE)
            {
                //this could be the place for uninit
            }
            if (lastGesture == GESTURE_TAP)
            {
                if (lastSelectedObject != nullptr && lastSelectedObject->GetIsSelectable())
                {
                    input.action = Input::SELECT_OBJECT;
                    input.object = lastSelectedObject;
                }
            }
            if (lastGesture == GESTURE_DRAG)
            {
                if (!dragStarted) {

                    if (lastSelectedObject != nullptr && lastSelectedObject->GetIsSelectable()) {
                        dragStarted = true;

                        input.action = Input::BEGIN_DRAG;
                        input.object = lastSelectedObject;
                    }
                }
                else
                {
                    if (previousSelectedObject == lastSelectedObject)
                    {
                        input.action = Input::CONTINUE_DRAG;
                        input.object = lastSelectedObject;
                    }
                    else
                    {
                        dragStarted = false;

                        input.action = Input::END_DRAG;
                        input.object = previousSelectedObject;
                    }
                }
            }
            else if (previousGesture == GESTURE_DRAG) {
                if (dragStarted) {
                    dragStarted = false;

                    input.action = Input::END_DRAG;
                    input.object = previousSelectedObject;
                }
            }
        }
    }
};



