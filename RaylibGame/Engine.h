#pragma once
#include "Types.h"
#include "Helpers.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include "Classes.h"

#define IF while
#define BREAK_IF break;

class Manager
{
public:
    static Container                    activeObjects;

};

Container Manager::activeObjects(SString("Active Objects"), { 0,0,0,0 });

class ScreenManager : public Manager {
    int                                 screenWidth = 1600;
    int                                 screenHeight = 800;

    const char*                         windowTitle = "raylib [core] example - mouse input";

public:
    ScreenManager();
    ScreenManager(int width, int height);
    void                                Init();
    void                                Draw();
    
    ~ScreenManager();
};
ScreenManager::ScreenManager() {
    Init();
}
ScreenManager::ScreenManager(int width, int height) : screenWidth(width), screenHeight(height) {
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
    int                                 gestureObtained;

    Vector2                             start;
    Vector2                             end;
};

class InputManager : public Manager {
public:
    int                                 enabledGestures = 0b0000000000001001;

    //Drag
    bool                                dragStarted = false;
    Owner                               dragSelectedObject = static_cast<GameObject*>(nullptr);
    Vector2                             mouseGrab = { 0,0 };
    float                               endPositionX = -1;
    float                               endPositionY = -1;
    float                               timeForDragDelay = 0.000000000001f;
    float                               temporayTimeForDragDelay = timeForDragDelay;
    float                               dragDuration = 0;

    InputManager(int enabled) : enabledGestures(enabled){}

    Input&                              ListenToInput();
};
Input& InputManager::ListenToInput() {
    Input input;

    int lastGesture = GetGestureDetected();
    Vector2 mouse = GetMousePosition();
    float delta = GetFrameTime();

    if(lastGesture == GESTURE_DRAG && !dragStarted) {

        //if drag just started -> init
        int order = 0;
        dragSelectedObject = GetSelectableObjectUnderPoint(mouse, activeObjects, order);

        if (dragSelectedObject != nullptr && dragSelectedObject->isSelectable)
        {
            //set focus - trebuie facuta treaba asta mai organizat, managed by action manager
            /*if (dragSelectedObject != nullptr && dragSelectedObject != hand.children[hand.children.empty() ? 0 : hand.children.size() - 1].go_pointer)
            {
                ResetPositionInArray<GameObject>(
                    hand.children,
                    *dragSelectedObject,
                    0,
                    hand.children.size(),
                    [](vector<Owner> &objArray, GameObject &obj)->bool
                {
                    obj.zIndex = (objArray[objArray.size() - 1].go_pointer)->zIndex + 1;
                    return true;
                },
                    &hand);
            }*/

            dragStarted = true;
            mouseGrab = { mouse.x - dragSelectedObject->position.x, mouse.y - dragSelectedObject->position.y };

            endPositionX = mouse.x;
            endPositionY = mouse.y;
        }
    }
    IF(dragStarted) {

        if ((dragSelectedObject == nullptr) ||
            (abs(endPositionX - dragSelectedObject->position.x) < 1 && abs(endPositionX - dragSelectedObject->position.x) >= 0.01f &&
                abs(endPositionY - dragSelectedObject->position.y) < 1 && abs(endPositionY - dragSelectedObject->position.y) >= 0.01f)) {//macro
               //end
            dragStarted = false;
            dragDuration = 0;
            temporayTimeForDragDelay = timeForDragDelay;
            BREAK_IF;
        }
        if ((abs(endPositionX - dragSelectedObject->position.x) == 0 && abs(endPositionY - dragSelectedObject->position.y) == 0)) {
            dragDuration = 0;
            temporayTimeForDragDelay = timeForDragDelay;
        }

        float lerp = 0;

        if (lastGesture == GestureType::GESTURE_DRAG)//pot aprea probleme aici ca nu este acelasi obiect
        {
            temporayTimeForDragDelay += delta;
            dragDuration += delta;
            lerp = dragDuration / temporayTimeForDragDelay;

            endPositionX = mouse.x - mouseGrab.x;
            endPositionY = mouse.y - mouseGrab.y;
        }
        else {
            dragDuration = (dragDuration + delta < temporayTimeForDragDelay) ? dragDuration + delta : temporayTimeForDragDelay;
            lerp = dragDuration / temporayTimeForDragDelay;
        }

        float deltaX = endPositionX - dragSelectedObject->position.x;
        float deltaY = endPositionY - dragSelectedObject->position.y;

        dragSelectedObject->position.x += deltaX * lerp;
        dragSelectedObject->position.y += deltaY * lerp;

        BREAK_IF;

    }
    if (lastGesture != GestureType::GESTURE_DRAG) {
        int order = 0;
        if (dragSelectedObject.go_pointer != GetObjectUnderPoint(mouse, activeObjects, order).go_pointer)
            dragStarted = false;
        else if (dragSelectedObject != nullptr)
            mouseGrab = { mouse.x - dragSelectedObject->position.x, mouse.y - dragSelectedObject->position.y };

        //deci avand in vedere faptul ca defapt poti identifica comportamentul de tragere prin secventa TAP,HOLD,DRAG
        //sunt necesare niste modificari => retin ultimul obiect tras(dragged) si daca se face actiunea hold peste el
        //atunci resetez mouseGrab. Ce ar fi fain aicea ar fi sa fie stocate aceste informatii in obiectul input hand
        //ler
    }
    if (lastGesture == GESTURE_NONE)
    {
        //this could be the place for uninit
    }

    input.gestureObtained = lastGesture;
    return input; // actually not good
}

struct Action
{
    int                                 action;
};

class ActionManager : public Manager {
public:
    Container                           saveStateObjects;

    Action&                             InterpretInput(const ::Input& input);
    void                                SaveState();
    void                                LoadState();
    void                                InterpretResponse(const Action& action);
};
Action& ActionManager::InterpretInput(const ::Input& input)
{
    Action action;

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