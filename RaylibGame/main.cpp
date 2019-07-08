#define _CRT_NONSTDC_NO_DEPRECATE
#include "nvidia.h"
#include "SString.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*


//"Card" MACRO_CONCAT("", __COUNTER__)

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be 
//easy to locate their place - perhaps some option added to the input manager


*/
//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

#define GET_NAME(x) #x //could be used in combination with SClose for printing variables after destruction of SClose object

#define CONCAT_NUM( x, y ) x#y
#define MACRO_CONCAT( x, y ) CONCAT_NUM( x, y )

#define iff while


#define INVALID_NEW_INDEX -1
#define ABSOLUT_NEW_INDEX -2

//-----[Classes]---------------------------------------------------------------------------------------------------------------------------------------------------------------

class GameObject;
class Card;
class Board;
class Container;
class CardContainer;

bool AddObjectToArray(vector<GameObject*> &activeObjects, GameObject &object);
template<typename T>
bool AddObjectToArray(vector<T*> &objectArray, T &object, int beginPos, int endPos);
bool ResetPositionInArray(vector<GameObject*> &activeObjects, GameObject &object, int newIndex = INVALID_NEW_INDEX);
template<typename T>
bool ResetPositionInArray(vector<T*> &objectArray, T &object, int beginPos, int endPos, int newIndex = INVALID_NEW_INDEX);

class GameObject {
public:
    SString::SString name;

    Rectangle position;
    Texture2D texture;
    Color color = LIGHTGRAY;
    int zIndex = 0;

    bool concedeDrawing = false;// this is used by objects like containers to manage sub objects
    bool isActive = true;

    GameObject() {

    }

    GameObject(SString::SString &name) : name(name){
        
    }

    virtual void Draw() {};
};

class Board : public GameObject {
public:

    Board() {
        
    }

    Board(SString::SString &name) : GameObject(name){
        
    }

    void Draw() { 
        DrawRectangleRec(position, color); 
    };
};

class Card : public GameObject {
public:

    Card() {

    }

    Card(SString::SString &name, Rectangle rectangle, Texture2D image) : GameObject(name) {
        position = rectangle;
        texture = image;
    }

    //ABILITY
    ///maybe add a function pointer for each
    ///or an event type

    void Draw() {
        DrawRectangleRec(position, color);
    }
};

class Container : public GameObject {
public:
    //vector<Container*> children;

    bool isZAranged = false;

    bool isMaterial = false;

    virtual void Draw() {}
};

class CardContainer : public Container {//perhaps cardcontainer might inherit from Container, a class that will be used mostly for the UI, but cardDB will have isMaterial = false, so it wont be shown
public:
    vector<Card*> cards;

    CardContainer() {

    }

    CardContainer(vector<Card*> cards) {
        this->cards = cards;
    }

    static vector<Card*> ExtractNCardsFrom(vector<Card*>& container, int n);

    bool Arange() {;

        int start = 0;
        int end = cards.size() - 1;
        for (auto obj : cards) {
            ResetPositionInArray<Card>(cards, *obj, start, end);
            start++;
        }

        return true;
    }

    void Draw() {
        if (!isMaterial)
            return;

        if (!isZAranged)
        {
            isZAranged = Arange();

            if (!isZAranged)
                return;
        }

        //draws himself first,
        DrawRectangleRec(position, color);

        //then the children
        auto iterator = cards.end() - 1;
        while (true) {
            (*iterator)->Draw();
            if (iterator == cards.begin())
                break;
            else {  
                iterator--;
            }
        }
    }
};

vector<Card*> CardContainer::ExtractNCardsFrom(vector<Card*>& container, int n)
{
    vector<Card*> selected;
    vector<int> selectedIndexes;

    if (n > container.size())
        return selected;

    for (int i = n; n >= 1;) {
        int randomN = GetRandomValue(0,container.size()-1);

        auto it = selectedIndexes.begin();
    loop:
        if (it == selectedIndexes.end())
            goto end;
        if ((*it) == randomN)
            continue;
        ++it;
        goto loop;
    end:
        selectedIndexes.push_back(randomN);
        n--;
    }

    for (auto it : selectedIndexes) {
        selected.push_back(container[it]);
    }

    return selected;
}

GameObject* GetObjectUnderPoint(Vector2 point, vector<GameObject*> &activeObjects, int order) {// e o problema aici, active Objects si Hand nu au aceeasi ordine
    vector<GameObject*>::iterator it;
    it = activeObjects.begin();
    while ( it != activeObjects.end()) {
        if ((*it)->isActive == true && CheckCollisionPointRec(point, (*it)->position))
        {
            --order;
        }
        if (order < 0)
            break;
        ++it;
    }

    if (it == activeObjects.end())
        --it;
    if (((*it)->zIndex == -1) || order >= 0)
        return nullptr;
    return *it;
}

bool AddObjectToArray(vector<GameObject*> &activeObjects, GameObject &object) {
    auto iterator = activeObjects.end() - 1;
    while (iterator != activeObjects.begin() && object.zIndex >= (*(*(iterator))).zIndex) {
        --iterator;
    }
    if (iterator == activeObjects.begin() && object.zIndex < (*(*(iterator))).zIndex) {
        iterator++;
    }

    //while (object.zIndex == (*(*iterator)).zIndex) {
    //    object.zIndex++;
    //    if (iterator == activeObjects.begin())
    //        break;
    //    --iterator;
    //    auto copyIterator = iterator;
    //    while (copyIterator != activeObjects.begin()) {
    //        ((*(*copyIterator)).zIndex)++;//sa trec prin toate si sa le cresc
    //        --copyIterator;
    //        if ((copyIterator) != activeObjects.begin())//chestia asta cred ca e retardata - o fac de 2 ori
    //            ((*(*(copyIterator-1))).zIndex)++;//problema e ca tre sa compar cu toate nu doar cu primu 4 ca primu 4 creste restul de 4 la 5 si ceilalti 5 vor fi 6 in urmatoarea runda si o sa fie mai mari
    //    }
    //}

    activeObjects.insert(iterator, &object);

    return true;
}

template<typename T>
bool AddObjectToArray(vector<T*> &objectArray, T &object, int beginPos, int endPos) {

    int idx = endPos;

    auto iterator = objectArray.begin();

    //auto begin = objectArray.begin();
    //auto iterator = begin + (endPos - 1);
    //auto start = begin + beginPos;

    while (idx != beginPos && object.zIndex >= (*iterator + idx)->zIndex) {
        --idx;
    }

    if (idx == beginPos && idx == endPos) {
        objectArray.insert(objectArray.end(), &object);
    }
    else {
        if (idx == beginPos && object.zIndex < (*(iterator + idx))->zIndex) {
            idx++;
        }
        objectArray.insert(iterator + idx, &object);
    }

    return true;
}

bool ResetPositionInArray(vector<GameObject*> &activeObjects, GameObject &object, int newIndex) {
    for (auto it = activeObjects.begin(); it != activeObjects.end(); ++it) {
        if ((*it) == &object) {
            activeObjects.erase(it);
            break;
        }
    }

    if (newIndex == ABSOLUT_NEW_INDEX)
        object.zIndex = activeObjects[0]->zIndex + 1;
    else if (newIndex != INVALID_NEW_INDEX)
        object.zIndex = newIndex;

    return AddObjectToArray(activeObjects, object);
}

template<typename T>
bool ResetPositionInArray(vector<T*> &objectArray, T &object, int beginPos , int endPos , int newIndex) {
    typename vector<T*>::iterator begin = objectArray.begin();

    //endPos must be the size of the array for the full iteration

    int idx = beginPos;
    for (; idx != endPos + 1; ++idx) {//this might break when it runs out of array
        if ((*(begin + idx)) == &object) {
            objectArray.erase(begin + idx);
            break;
        }
    }

    if (newIndex == ABSOLUT_NEW_INDEX)
        object.zIndex = objectArray[0]->zIndex + 1;
    else if (newIndex != INVALID_NEW_INDEX)
        object.zIndex = newIndex;

    if (idx < beginPos)
    {
        beginPos--; endPos--;
    }
    else if (idx < endPos)
        endPos--;

    return AddObjectToArray<T>(objectArray, object, beginPos, endPos);
}

//-----[Globals]---------------------------------------------------------------------------------------------------------------------------------------------------------------

const int screenWidth = 1600;
const int screenHeight = 800;

const int FONT_SIZE = 50;

//Input Globals
constexpr float timeForDragDelay = 0.1f;
static_assert(timeForDragDelay != 0, "timeForDragDelay must not be 0");
float temporayTimeForDragDelay = timeForDragDelay;
float dragDuration = 0;
Vector2 mouseGrab = { 0,0 };
bool DragStarted = false;
GameObject* dragSelectedObject = nullptr;//current selected object
float endPositionX = -1;
float endPositionY = -1;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(void)
{
    /*-----[SO INITIALIZATION]-----------------------------------------------------------------------------------------------------------------------------*/

    InitWindow(screenWidth, screenHeight, "Ciopillis");
    SetTargetFPS(60);
    //SleepFunc(0.1f);

    /*-----[SOFT INITIALIZATION]-----------------------------------------------------------------------------------------------------------------------------*/

    Vector2 touchPosition = { 0, 0 };
    Rectangle touchArea = { 0, 0, screenWidth, screenHeight };

    int gesturesEnabled = 0b0000000000001001;
    SetGesturesEnabled(gesturesEnabled);

    /*-----[GAME INITIALIZATION]-----------------------------------------------------------------------------------------------------------------------------*/
    /*-----[RESOURCES]----------------------------------------------------------*/
    
    vector<GameObject*> activeObjects;

    CardContainer cardDatabase;
    cardDatabase.isMaterial = false;

    SString::SString nume("test");
    Board board;
    board.name = nume;
    board.position = { 0, 0, screenWidth, screenHeight };
    board.zIndex = -1;
    activeObjects.push_back(&board);

    SString::SString numeCarte("Carte");
    Texture2D texture = { 0 };
    Card card(numeCarte, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture);
    card.zIndex = 1;
    card.color = GREEN;
    AddObjectToArray(activeObjects, card);

    SString::SString numeCarte1("Carte1");
    Texture2D texture1 = { 0 };
    Card card1(numeCarte1, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture1);
    card1.zIndex = 2;
    card1.color = BLUE;
    AddObjectToArray(activeObjects, card1);

    SString::SString numeCarte2("Carte2");
    Texture2D texture2 = { 0 };
    Card card2(numeCarte2, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture2);
    card2.zIndex = 2;
    card2.color = RED;
    AddObjectToArray(activeObjects, card2);

    SString::SString numeCarte3("Carte3");
    Texture2D texture3 = { 0 };
    Card card3(numeCarte3, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture3);
    card3.zIndex = 3;
    card3.color = BLACK;
    AddObjectToArray(activeObjects, card3);

    SString::SString numeCarte4("Carte4");
    Texture2D texture4 = { 0 };
    Card card4(numeCarte4, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture4);
    card4.zIndex = 4;
    card4.color = PINK;
    AddObjectToArray(activeObjects, card4);

    Board board1;
    board1.zIndex = 2;
    Board board2;
    board2.zIndex = 4;
    Board board3(nume.Substitute("Board3"));
    board3.zIndex = 4;
    Board board4;
    board4.zIndex = 4;
    Board board5(nume.Substitute("Board5"));
    board5.zIndex = 4;

    AddObjectToArray(activeObjects, board1);
    AddObjectToArray(activeObjects, board2);
    AddObjectToArray(activeObjects, board3);
    AddObjectToArray(activeObjects, board4);
    AddObjectToArray(activeObjects, board5);

    ResetPositionInArray(activeObjects, board3, ABSOLUT_NEW_INDEX);

    //////////////
    cardDatabase.cards.push_back(&card);
    cardDatabase.cards.push_back(&card1);
    cardDatabase.cards.push_back(&card2);
    cardDatabase.cards.push_back(&card3);
    cardDatabase.cards.push_back(&card4);

    /*-----[GAME]-----------------------------------------------------------------------------------------------------------------------------*/

    CardContainer hand(CardContainer::ExtractNCardsFrom(cardDatabase.cards, 3));
    hand.isMaterial = true;

    for (auto card : cardDatabase.cards) {
        card->concedeDrawing = true;
        card->isActive = false;
    }
    //so every card becomes inactive and a child
    for (auto card : hand.cards) {
        card->isActive = true;
    }
    //and every child in hand is now active
    AddObjectToArray(activeObjects,hand);//this should be a gamemanager object function

    while (!WindowShouldClose())
    {
        int lastGesture = GetGestureDetected();

        Vector2 mouse = GetMousePosition();
        float delta = GetFrameTime();

        system("CLS");

        //if lastGesture != gesture none inseamna ca userul face ceva, deci trebuie salvata inainte locatia cartilor si apoi dupa ce se reincepe logica,
        //trebuie validata pozitia si restabilita memoria

        iff(!DragStarted && lastGesture == GestureType::GESTURE_DRAG) {

            //if drag just started -> init
            dragSelectedObject = GetObjectUnderPoint(GetMousePosition(), activeObjects, 0);//aici e problema - nu este modificata ordinea din activeObjects - cea mai usoara metoda este 
            //sa faci active Objects ca un obiect cu mai multi fii de tip container (deci sa nu mai fie doar vector), iar fiecare copil are grija sa se deseneze pe sine si pe copii sai recursiv
            //trebuie de asemenea vazut daca mai intai desenam containerul sau obiectele copil (zContainer < zCopii)
            if (dragSelectedObject == nullptr)
                break;

            //set focus - trebuie facuta treaba asta mai organizat, managed by input manager
            if (dragSelectedObject != nullptr && dragSelectedObject != hand.cards[0])
            {
                ResetPositionInArray<Card>(hand.cards, *((Card*)dragSelectedObject), 0, hand.cards.size());
                hand.isZAranged = false;
            }

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
            if (!(*iterator)->concedeDrawing)
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

