#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include "nvidia.h"
#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#define FONT_SIZE 50

class GameObject {
public:
    Rectangle position;
    Texture2D texture;
    int zIndex = 0;

    virtual void Draw() {};
};

class Board : public GameObject {
public:
    char name[10] = "board";

    void Draw() { DrawRectangleRec(position, RED); };
};

class Card : public GameObject {
public:
    Color color = MAROON;
    char name[10] = "card";

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

GameObject* GetObjectUnderPoint(Vector2 point, vector<GameObject*> &activeObjects, int order) {
    vector<GameObject*>::iterator it;

    for (it = activeObjects.begin(); it != activeObjects.end(); ++it) {
        if (CheckCollisionPointRec(point, (*it)->position))
        {
            --order;
        }
        if (order < 0)
            break;
    }

    if ((*it)->zIndex == -1)
        return nullptr;
    else
        return *it;
}

bool AddObjectToArray(vector<GameObject*> &activeObjects, GameObject &object) {
    auto iterator = activeObjects.end() - 1;
    while ((iterator) != activeObjects.begin() && object.zIndex > (*(*(iterator))).zIndex) {
        if (object.zIndex >= (*(*(iterator - 1))).zIndex)
            --iterator;
        else
            break;
    }

    while (object.zIndex == (*(*iterator)).zIndex) {
        object.zIndex++;
        if (iterator == activeObjects.begin())
            break;
        --iterator;
        auto copyIterator = iterator;
        while (copyIterator != activeObjects.begin()) {
            ((*(*copyIterator)).zIndex)++;
            --copyIterator;
            if ((copyIterator) != activeObjects.begin())
                ((*(*(copyIterator-1))).zIndex)++;//problema e ca tre sa compar cu toate nu doar cu primu 4 ca primu 4 creste restul de 4 la 5 si ceilalti 5 vor fi 6 in urmatoarea runda si o sa fie mai mari
        }
    }

    activeObjects.insert(iterator, &object);

    return true;
}

int main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - keyboard input");

    SetTargetFPS(60);

    /////////

    Vector2 touchPosition = { 0, 0 };
    Rectangle touchArea = { 0, 0, screenWidth, screenHeight };

    int gesturesEnabled = 0b0000000000001001;
    SetGesturesEnabled(gesturesEnabled);

    Texture2D texture = { 0 };
    Card card({ screenWidth / 2,screenHeight / 2, 200 , 200 }, texture);//ar trebui alocate dinamic
    card.zIndex = 0;

    vector<GameObject*> activeObjects;

    Board board;//ar trebui alocate dinamic
    board.position = { 0, 0, 0, 0 };
    board.zIndex = -1;
    activeObjects.push_back(&board);

    AddObjectToArray(activeObjects,card);

    Board board1;
    board1.zIndex = 2;
    Board board2;
    board2.zIndex = 4;
    Board board3;
    board3.zIndex = 4;
    Board board4;
    board4.zIndex = 4;
    Board board5;
    board5.name[0] = 'a';
    board5.zIndex = 4;

    AddObjectToArray(activeObjects, board1);
    AddObjectToArray(activeObjects, board2);
    AddObjectToArray(activeObjects, board3);
    AddObjectToArray(activeObjects, board4);
    AddObjectToArray(activeObjects, board5);

    float timeForDragDelay = 2.0f;
    float dragDuration = 0;

    while (!WindowShouldClose())
    {
        int lastGesture = GetGestureDetected();
        if (lastGesture != GestureType::GESTURE_NONE)
        {
            if (lastGesture == GestureType::GESTURE_DRAG) {

                GameObject* object = GetObjectUnderPoint(GetMousePosition(), activeObjects, 0);

                Vector2 mouse = GetMousePosition();

                int textOffset = 0;

                float delta = GetFrameTime();
                dragDuration = (dragDuration + delta < timeForDragDelay) ? dragDuration + delta : timeForDragDelay;

                char _text_delta[30];
                itoa(delta, _text_delta, 10);

                DrawText(_text_delta, 0, 0, FONT_SIZE, BLACK);
                textOffset += FONT_SIZE;

                char _text_ddur[30];
                itoa(dragDuration, _text_ddur, 10);

                DrawText(_text_ddur, 0, 0, FONT_SIZE, BLACK);
                textOffset += FONT_SIZE;

                if (object != nullptr) {
                    float endPositionX = mouse.x - object->position.width / 2;
                    float endPositionY = mouse.y - object->position.height / 2;

                    char _text_end[30];
                    itoa(endPositionX, _text_end, 10);
                    itoa(endPositionY, _text_end + strlen(_text_end), 10);

                    DrawText(_text_end, 0, 0, FONT_SIZE, BLACK);
                    textOffset += FONT_SIZE;

                    float deltaX = mouse.x - (object->position.width / 2) - object->position.x;
                    float deltaY = mouse.y - (object->position.height / 2) - object->position.y;

                    char _text_del[30];
                    itoa(deltaX, _text_del, 10);
                    itoa(deltaY, _text_del + strlen(_text_del), 10);

                    DrawText(_text_del, 0, 0, FONT_SIZE, BLACK);
                    textOffset += FONT_SIZE;

                    object->position.x = endPositionX  + deltaX * (dragDuration / timeForDragDelay);
                    object->position.y = endPositionY + deltaY * (dragDuration / timeForDragDelay);
                }
            }
            else {
                dragDuration = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        auto iterator = activeObjects.end() - 1;
        while (true) {
            (*iterator)->Draw();
            if (iterator == activeObjects.begin())
                break;
            else
                iterator--;
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
