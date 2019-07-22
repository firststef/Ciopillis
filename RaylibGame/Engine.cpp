#include "Engine.h"

ScreenManager::ScreenManager(int width, int height, Container& activeObj) : screenWidth(width), screenHeight(height), activeObjects(activeObj) {
    Init();
}
void ScreenManager::Init()
{
    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);
}
void ScreenManager::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (auto obj = activeObjects.children.begin(); obj != activeObjects.children.end(); ++obj) {

        if (!(*obj).index) {
            auto pointer = (*obj).go_pointer;
            if (pointer->isActive)
                pointer->Draw();
        }
        else {
            auto pointer = (*obj).c_pointer;
            if (pointer->isActive)
                pointer->Draw();
        }
    }

    EndDrawing();
}
ScreenManager::~ScreenManager() {
    CloseWindow();
}

InputManager::InputManager(int enabled, Container& activeObj) : enabledGestures(enabled), activeObjects(activeObj)
{
    SetGesturesEnabled(enabledGestures);
}
Input InputManager::ListenToInput() {
    Input input;

    int lastGesture = GetGestureDetected();
    Vector2 mouse = GetMousePosition();
    float delta = GetFrameTime();

    int selectedOrder = 0;

    lastSelectedObject = GetSelectableObjectUnderPoint(mouse, activeObjects, selectedOrder);
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
        if (lastSelectedObject != nullptr && lastSelectedObject->isSelectable)
        {
            input.action = Input::SELECT_OBJECT;
            input.object = lastSelectedObject;
        }
    }
    if (lastGesture == GESTURE_DRAG)
    {
        if (!dragStarted) {

            if (lastSelectedObject != nullptr && lastSelectedObject->isSelectable) {
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
}
void InputManager::ResetLastSelected()
{
    previousSelectedObject = static_cast<GameObject*>(nullptr);
}

ActionManager::ActionManager(Container& activeObj) : activeObjects(activeObj)
{

}
Action& ActionManager::InterpretInput(Input& input)
{
    Action action;

    Vector2 mouse = GetMousePosition();

    if (input.action != Input::IDLE)
    {
        //SaveState();
    }

    if (input.action == Input::SELECT_OBJECT)
    {
        cout << input.object.go_pointer->name << "is selected" << endl;
        //aici ar veni input.object.on_select(); on_select fiind o valoare de lambda pentru ce se intampla la selectie

        if (input.object.parent->children[input.object.parent->children.empty() ? 0 : input.object.parent->children.size() - 1] != input.object)
        {
            ResetPositionInContainer(
                *input.object.parent,
                input.object,
                0,
                input.object.parent->children.size() - 1,
                [](Container &cont, Owner &obj)->bool
            {
                obj.go_pointer->zIndex = (cont.children.empty()) ? 1 : (cont.children[cont.children.size() - 1].go_pointer)->zIndex + 1;
                return true;
            }
            );
        }
    }
    if (input.action == Input::BEGIN_DRAG)
    {
        cout << input.object.go_pointer->name << "begin drag" << endl;
        //input.object.on_begin_drag();

        mouseGrab = { mouse.x - input.object.go_pointer->position.x, mouse.y - input.object.go_pointer->position.y };

        //aici in interiorul on_begin_drag() cel mai probabil se va apela un refresh pentru parinte
    }
    if (input.action == Input::CONTINUE_DRAG)
    {
        cout << input.object.go_pointer->name << "continue drag" << endl;
        //input.object.on_drag();

        endPos = { mouse.x - mouseGrab.x, mouse.y - mouseGrab.y };

        input.object.go_pointer->position = { endPos.x, endPos.y, input.object.go_pointer->position.width, input.object.go_pointer->position.height };

        //check peste ce trece, apeleaza - on_dragged_over();
        //de asemenea trebuie retinut obiectul peste care a trecut, daca s-a schimbat, ar trebui apelat on_end_dragged_over();
    }
    if (input.action == Input::END_DRAG)
    {
        cout << input.object.go_pointer->name << "end drag" << endl;
        //input.object.on_end_drag();

        endPos = { mouse.x - mouseGrab.x, mouse.y - mouseGrab.y };

        input.object.go_pointer->position = { endPos.x, endPos.y, input.object.go_pointer->position.width, input.object.go_pointer->position.height };

        //trebuie verificat acum peste ce este released - on_release_over();
    }

    return action;
}
void ActionManager::SaveState()
{
    saveStateObjects.Destroy();
    saveStateObjects = *activeObjects.GetCopy();
}
void ActionManager::LoadState()
{
    activeObjects.Destroy();
    activeObjects = saveStateObjects;
}
void ActionManager::InterpretResponse(const Action& action)
{

}

Action& GameManager::ValidateAction(const Action& action)
{
    Action response;

    return response;
}