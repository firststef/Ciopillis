#pragma once
#include "Types.h"
#include "Helpers.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include "Classes.h"
#include "Server.h"

class ScreenManager {
    int                                 screenWidth = 1600;
    int                                 screenHeight = 800;

    const char*                         windowTitle = "raylib [core] example - mouse input";

    Container&                          activeObjects;

public:
    ScreenManager(int width, int height, Container& activeObj);
    void                                Init();
    void                                Draw();
    
    ~ScreenManager();
};
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

struct Input {
    enum InputAction
    {
        IDLE = 0,
        SELECT_OBJECT,
        BEGIN_DRAG,
        CONTINUE_DRAG,
        END_DRAG
    }                                   action = IDLE;


    int                                 gestureObtained;
    Owner                               object = static_cast<GameObject*>(nullptr);

    Vector2                             start;
    Vector2                             end;
};

class InputManager {
public:
    Container&                          activeObjects;

    int                                 enabledGestures = 0b1111111111111111;

    Owner                               previousSelectedObject = static_cast<GameObject*>(nullptr);
    Owner                               lastSelectedObject = static_cast<GameObject*>(nullptr);
    int                                 previousGesture = GESTURE_NONE;

    //Tap

    //Drag
    bool                                dragStarted = false;

    InputManager(int enabled, Container& activeObj);

    Input                              ListenToInput();
    void                                ResetLastSelected();
};
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
    if(lastGesture == GESTURE_TAP)
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

class Animation
{
    double                              startTime = 0;
    double                              endTime = 0;
};

struct Action
{
    enum ActionType
    {
        UNASSIGNED,
        INPUT_ACTION,
        GAME_ACTION
    }                                   type = UNASSIGNED;
};

class ActionManager {
public:
    Container&                          activeObjects;

    Container                           saveStateObjects;

    //Select

    //Drag
    Vector2                             mouseGrab;
    Vector2                             endPos;

    ActionManager(Container& activeObj);

    Action&                             InterpretInput( Input& input);
    void                                SaveState();
    void                                LoadState();
    void                                InterpretResponse(const Action& action);
};
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
    if(input.action == Input::BEGIN_DRAG)
    {
        cout << input.object.go_pointer->name << "begin drag" << endl;
        //input.object.on_begin_drag();

        mouseGrab = { mouse.x - input.object.go_pointer->position.x, mouse.y - input.object.go_pointer->position.y };

        //aici in interiorul on_begin_drag() cel mai probabil se va apela un refresh pentru parinte
    }
    if(input.action == Input::CONTINUE_DRAG)
    {
        cout << input.object.go_pointer->name << "continue drag" << endl;
        //input.object.on_drag();

        endPos = { mouse.x - mouseGrab.x, mouse.y - mouseGrab.y };

        input.object.go_pointer->position = { endPos.x, endPos.y, input.object.go_pointer->position.width, input.object.go_pointer->position.height };

        //check peste ce trece, apeleaza - on_dragged_over();
        //de asemenea trebuie retinut obiectul peste care a trecut, daca s-a schimbat, ar trebui apelat on_end_dragged_over();
    }
    if(input.action == Input::END_DRAG)
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

class GameManager {
public:
    Action&                              ValidateAction(const Action& action);
};
Action& GameManager::ValidateAction(const Action& action)
{
    Action response;

    return response;
}

class Manager
{
public:
    Container                           activeObjects;

    Entity                              playerOne;
    Entity                              playerTwo;

    ScreenManager                       screenManager;
    GameManager                         gameManager;
    InputManager                        inputManager;
    ActionManager                       actionManager;

    Manager(int screenWidth = 1600, int screenHeight = 900, int enabledGestures = 0b0000000000001111) :
        activeObjects(SString("Active Objects"), {0,0,0,0}),
        playerOne(SString("playerOne")), playerTwo(SString("playerTwo")),
        screenManager(screenWidth,screenHeight,activeObjects),
        inputManager(enabledGestures,activeObjects),
        actionManager(activeObjects)
    {
    }

    ~Manager() { activeObjects.Destroy(); }
};