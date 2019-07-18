#pragma once
#include "Types.h"
#include "Helpers.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include "Classes.h"

class Manager
{
public:
    static Container activeObjects;

};

class ScreenManager : public Manager {
    int screenWidth = 1600;
    int screenHeight = 800;

    const char* windowTitle = "raylib [core] example - mouse input";

public:

    ScreenManager() {
        Init();
    }

    ScreenManager(int width, int height) : screenWidth(width), screenHeight(height) {
        Init();
    }

    void Init()
    {
        InitWindow(screenWidth, screenHeight, windowTitle);
        SetTargetFPS(60);
    }

    ~ScreenManager() {
        CloseWindow();
    }
};

struct Input {
    int gestureObtained;

    Vector2 start;
    Vector2 end;
};

class InputManager : public Manager {
public:
    int enabledGestures = 0b0000000000001001;

    //Drag
    bool dragStarted = false;
    Owner dragSelectedObject = static_cast<GameObject*>(nullptr);
    Vector2 mouseGrab = { 0,0 };
    float endPositionX = -1;
    float endPositionY = -1;
    float timeForDragDelay = 0.000000000001f;
    float temporayTimeForDragDelay = timeForDragDelay;
    float dragDuration = 0;

    InputManager(int enabled) : enabledGestures(enabled)
    {

    }

    Input& ListenToInput();
};

Input& InputManager::ListenToInput() {
    Input input;

    int lastGesture = GetGestureDetected();
    Vector2 mouse = GetMousePosition();
    float delta = GetFrameTime();

    iff(!dragStarted && lastGesture == GESTURE_DRAG) {

        //if drag just started -> init
        int order = 0;
        dragSelectedObject = GetObjectUnderPoint(mouse, activeObjects, order);

        if (dragSelectedObject == nullptr)
            break;

        //set focus - trebuie facuta treaba asta mai organizat, managed by input manager
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
        break;
    }
    iff(dragStarted) {

        if ((dragSelectedObject == nullptr) ||
            (abs(endPositionX - dragSelectedObject->position.x) < 1 && abs(endPositionX - dragSelectedObject->position.x) >= 0.01f &&
                abs(endPositionY - dragSelectedObject->position.y) < 1 && abs(endPositionY - dragSelectedObject->position.y) >= 0.01f)) {//macro
               //end
            dragStarted = false;
            dragDuration = 0;
            temporayTimeForDragDelay = timeForDragDelay;
            break;
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

        break;

    }
    if (lastGesture != GestureType::GESTURE_DRAG) {
        int order = 0;
        if (dragSelectedObject != GetObjectUnderPoint(mouse, activeObjects, order))
            dragStarted = false;
        else if (dragSelectedObject != nullptr)
            mouseGrab = { mouse.x - dragSelectedObject->position.x, mouse.y - dragSelectedObject->position.y };

        //deci avand in vedere faptul ca defapt poti identifica comportamentul de tragere prin secventa TAP,HOLD,DRAG
        //sunt necesare niste modificari => retin ultimul obiect tras(dragged) si daca se face actiunea hold peste el
        //atunci resetez mouseGrab. Ce ar fi fain aicea ar fi sa fie stocate aceste informatii in obiectul input hand
        //ler
    }
    if (lastGesture == GestureType::GESTURE_NONE)
    {
        //this could be the place for uninit
    }

    input.gestureObtained = lastGesture;
    return input; // actually not good
}

struct Action
{
    int action;
};

class ActionManager : public Manager {
public:
    Container saveStateObjects;

    Action& InterpretInput(const ::Input& input);
    void SaveState();
    void LoadState();
};

Action& ActionManager::InterpretInput(const ::Input& input)
{
    Action action;

    return action;
}

void ActionManager::SaveState()
{
    for auto 
}

void ActionManager::LoadState()
{

}

class GameManager {
    vector <Owner>  activeObjects;

};