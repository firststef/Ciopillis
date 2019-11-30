#pragma once
#include "Component.h"

struct InputComponent : IComponent
{
    void Init() override {}
};

Input input;

int lastGesture = GetGestureDetected();
Vector2 mouse = GetMousePosition();
float delta = GetFrameTime();

int selectedOrder = 0;

lastSelectedObject = activeObjects.GetSelectableObjectUnderPoint(mouse, selectedOrder);
//if (lastSelectedObject == nullptr || (lastSelectedObject != nullptr && !lastSelectedObject->isSelectable))--not allowed for check here - when nullptr comes it has to collide to some conditions
//{
//    input.gestureObtained = lastGesture;
//    previousGesture = lastGesture;
//    previousSelectedObject = lastSelectedObject;
//    return input;
//}

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

input.gestureObtained = lastGesture;
previousGesture = lastGesture;
previousSelectedObject = lastSelectedObject;
return input;