#pragma once
#include <iostream>
#include <vector>
#include "raylib.h"
#include "Classes.h"
#include "Server.h"

class ScreenManager {
public:
    int                                 screenWidth = 1600;
    int                                 screenHeight = 800;

    const char*                         windowTitle = "raylib [core] example - mouse input";

    Container&                          activeObjects;


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
    Owner*                              object;

    Vector2                             start;
    Vector2                             end;
};

class InputManager {
public:
    Container&                          activeObjects;

    int                                 enabledGestures = 0b1111111111111111;

    Owner*                              previousSelectedObject;
    Owner*                              lastSelectedObject;
    int                                 previousGesture = GESTURE_NONE;

    //Tap

    //Drag
    bool                                dragStarted = false;

    InputManager(int enabled, Container& activeObj);

    Input                               ListenToInput();
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

    //Container                         saveStateObjects;

    //Select

    //Drag
    Vector2                             mouseGrab;
    Vector2                             endPos;

    ActionManager(Container& activeObj);

    Action                              InterpretInput( Input& input);
    void                                SaveState();
    void                                LoadState();
    void                                InterpretResponse(const Action& action);
};

class GameManager {
public:
    Action                              ValidateAction(const Action& action);
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
        activeObjects(std::string("Active Objects"), 0, PINK, {0,0,0,0},Container::ContainerType::LOGICAL),
        playerOne(std::string("playerOne")), playerTwo(std::string("playerTwo")),
        screenManager(screenWidth,screenHeight,activeObjects),
        inputManager(enabledGestures,activeObjects),
        actionManager(activeObjects)
    {
    }

    ~Manager() {}
};