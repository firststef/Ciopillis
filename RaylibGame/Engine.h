#pragma once
#include <iostream>
#include <vector>
#include "raylib.h"
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

class GameManager {
public:
    Action&                              ValidateAction(const Action& action);
};

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
        activeObjects(Types::SString("Active Objects"), {0,0,0,0}),
        playerOne(Types::SString("playerOne")), playerTwo(Types::SString("playerTwo")),
        screenManager(screenWidth,screenHeight,activeObjects),
        inputManager(enabledGestures,activeObjects),
        actionManager(activeObjects)
    {
    }

    ~Manager() { activeObjects.Destroy(); }
};