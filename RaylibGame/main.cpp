#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include "nvidia.h"
#include "raylib.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be 
//easy to locate their place - perhaps some option added to the input manager

#define FONT_SIZE 50
#define iff while

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
    it = activeObjects.begin();
    while ( it != activeObjects.end()) {
        if (CheckCollisionPointRec(point, (*it)->position))
        {
            --order;
        }
        if (order < 0)
            break;
        ++it;
    }

    if (it == activeObjects.end())
        --it;
    if (((*it)->zIndex == -1))
        return nullptr;
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
            ((*(*copyIterator)).zIndex)++;//sa trec prin toate si sa le cresc
            --copyIterator;
            if ((copyIterator) != activeObjects.begin())//chestia asta cred ca e retardata - o fac de 2 ori
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
    board.position = { 0, 0, screenWidth, screenHeight };
    board.zIndex = -1;
    activeObjects.push_back(&board);

    AddObjectToArray(activeObjects, card);

    Texture2D texture1 = { 0 };
    Card card1({ screenWidth / 2,screenHeight / 2, 30 , 300 }, texture1);//ar trebui alocate dinamic
    card1.zIndex = 1;

    AddObjectToArray(activeObjects, card1);

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

    constexpr float timeForDragDelay = 0.1f;
    static_assert(timeForDragDelay != 0, "timeForDragDelay must not be 0");
    float temporayTimeForDragDelay = timeForDragDelay;
    float dragDuration = 0;
    Vector2 mouseGrab = { 0,0 };
    bool DragStarted = false;
    GameObject* dragSelectedObject = nullptr;//current selected object
    float endPositionX = -1;
    float endPositionY = -1;

    while (!WindowShouldClose())
    {
        int lastGesture = GetGestureDetected();

        Vector2 mouse = GetMousePosition();
        float delta = GetFrameTime();

        system("CLS");
        cout << dragSelectedObject << endl;
        cout << "Dragstarted" << DragStarted << endl;
        cout << "MouseGrab" << mouseGrab.x << "," << mouseGrab.y << endl;
        cout << "Delta " << delta << endl;
        cout << "DragDuration " << dragDuration << endl;
        cout << "temporayTimeForDragDelay " << temporayTimeForDragDelay << endl;

        iff(!DragStarted && lastGesture == GestureType::GESTURE_DRAG) {

            //if drag just started -> init
            dragSelectedObject = GetObjectUnderPoint(GetMousePosition(), activeObjects, 0);
            if (dragSelectedObject == nullptr)
                break;

            DragStarted = true;
            mouseGrab = { mouse.x - dragSelectedObject->position.x, mouse.y - dragSelectedObject->position.y };

            endPositionX = mouse.x;
            endPositionY = mouse.y;
            break;
        }
        iff(DragStarted) {

            if ((dragSelectedObject == nullptr) ||
                (abs(endPositionX - dragSelectedObject->position.x) < 1 && abs(endPositionX - dragSelectedObject->position.x) >= 0.01f &&
                    abs(endPositionY - dragSelectedObject->position.y) < 1 && abs(endPositionY - dragSelectedObject->position.y) >= 0.01f)) {//macro
                   //end
                DragStarted = false;
                dragDuration = 0;
                temporayTimeForDragDelay = timeForDragDelay;
                break;
            }
            if  ((abs(endPositionX - dragSelectedObject->position.x) == 0 && abs(endPositionY - dragSelectedObject->position.y) == 0)){
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

                cout << "EndPosition " << endPositionX << "," << endPositionY << endl;
            }
            else {
                dragDuration = (dragDuration + delta < temporayTimeForDragDelay) ? dragDuration + delta : temporayTimeForDragDelay;
                lerp = dragDuration / temporayTimeForDragDelay;
            }

            float deltaX = endPositionX - dragSelectedObject->position.x;
            float deltaY = endPositionY - dragSelectedObject->position.y;

            cout << "Distance " << deltaX << "," << deltaY << endl;

            dragSelectedObject->position.x += deltaX * lerp;
            dragSelectedObject->position.y += deltaY * lerp;

            break;

        }
        if (lastGesture != GestureType::GESTURE_DRAG) {
            if (dragSelectedObject != GetObjectUnderPoint(GetMousePosition(), activeObjects, 0))
                DragStarted = false;
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