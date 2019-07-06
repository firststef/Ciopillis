#include "raylib.h"
#include "nvidia.h"
#include <vector>

using namespace std;

class GameObject {
public:
    Rectangle position;
    Texture2D texture;

    int zIndex;
    bool IsActive;
    virtual void Draw();
};

class Board {

};

class Card : public GameObject {
public:
    Card(Rectangle rectangle, Texture2D image) {
        position = rectangle;
        texture = image;
    }

    //ABILITY
    ///maybe add a function pointer for each
    ///or an event type

    void Draw();
};

void Card::Draw() {
    DrawRectangleRec(position, MAROON);
}

//every object should have an internal function that repeats drawing until the object is destroyed or hidden

class ScreenManager {
    int screenWidth = 1600;
    int screenHeight = 800;

    const char* windowTitle = "raylib [core] example - mouse input";

public:

    ScreenManager() {
        InitWindow(screenWidth, screenHeight, windowTitle);
        SetTargetFPS(60);
    }

    ScreenManager(int width, int height) {
        InitWindow(width, height, windowTitle);
        SetTargetFPS(60);
    }

    ~ScreenManager() {
        CloseWindow();
    }
};

class EventManager {
    ///list of events
};

struct Input {
    int gestureObtained;

    Vector2 start;
    Vector2 end;

    vector<GameObject> objectsMet;
};

class InputManager {
    int enabledGestures = 0b0000000000001001;

    Vector2 touchPosition = { 0, 0 };
    Rectangle touchArea = { 0, 0, 0 , 0 };

    int currentGesture = GESTURE_NONE;
    int lastGesture = GESTURE_NONE;
public:
    InputManager(Rectangle rectangle) {
        touchArea = rectangle;
        SetGesturesEnabled(enabledGestures);
    }

    Input& ListenToInput();
};

Input& InputManager::ListenToInput() {
    Input input;
    input.gestureObtained = GESTURE_NONE;
    input.start = touchPosition;

    lastGesture = currentGesture;
    currentGesture = GetGestureDetected();
    touchPosition = GetTouchPosition(0);

    if (CheckCollisionPointRec(touchPosition, touchArea) && (currentGesture != GESTURE_NONE))
    {
        if ((currentGesture & enabledGestures) > 0)
            input.gestureObtained = currentGesture;
        ///eventual aici se pot verifica inputurile care sunt permise dar sunt invalide
    }

    input.end = touchPosition;

    return input; // actually not good
}

class GameManager {
    vector<GameObject> activeObjects;
public:
    GameManager() {
        LoadGame();
    }

    bool LoadGame();//ce parametru? path? file?
    void RunGame();
};

bool GameManager::LoadGame() {
    ///aici se fac niste initializari
}

GameObject* GetObjectUnderPoint(Vector2 point, vector<GameObject> &activeObjects, int order) {
    auto iterator = activeObjects.end();
    while (iterator != activeObjects.begin() && CheckCollisionPointRec(point, (*iterator).position)) {
        --iterator;
    }

    if ((*iterator).zIndex == -1)
        return nullptr;
    else
        return &(*iterator);
}

bool AddObjectToArray(vector<GameObject> &activeObjects, GameObject &object, int zIndex = 0) {
    auto iterator = activeObjects.end();
    while (iterator != activeObjects.begin() && object.zIndex > (*iterator).zIndex) {
        --iterator;
    }

    if (object.zIndex == (*iterator).zIndex) {
        auto copyIterator = iterator;
        while (copyIterator != activeObjects.end()) {
            ((*copyIterator).zIndex)++;
            ++copyIterator;
        }
    }

    activeObjects.insert(iterator, object);
}

void GameManager::RunGame() {
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //Visual section
        ///Screen manager should handle this
        BeginDrawing();
        for (auto object : activeObjects)
        {
            object.Draw();
        }
        EndDrawing();
        ///each section should request access from the others - int available = object.Draw(); -> if (available) Input.Get()
        //Input section 
        int lastGesture = GetGestureDetected();
        if (lastGesture != GestureType::GESTURE_NONE && lastGesture & enabledGestures)
        {
            switch (lastGesture) {
            case GESTURE_TAP:
                break;
            case GESTURE_DOUBLETAP:
                break;
            case GESTURE_HOLD:
                break;
            case GESTURE_DRAG:
                break;
            case GESTURE_SWIPE_RIGHT:
                break;
            case GESTURE_SWIPE_LEFT:
                break;
            case GESTURE_SWIPE_UP:
                break;
            case GESTURE_SWIPE_DOWN:
                break;
            case GESTURE_PINCH_IN:
                break;
            case GESTURE_PINCH_OUT:
                break;
            default: break;
            }
            Vector2 drag = GetGestureDragVector();

            GameObject& object = &card;//GameManager.GetHoveredObject()+


            Vector2 mouse = GetMousePosition();

            object->position.x = mouse.x - card.position.width / 2;
            object->position.y = mouse.y - card.position.height / 2;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        card.Draw();
        EndDrawing();

        //Computation section
    }
}

int main(void)
{
    ScreenManager screenManager;

    GameManager gameManager;
    gameManager.RunGame();
    return 0;
}