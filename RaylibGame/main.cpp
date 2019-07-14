#define _CRT_NONSTDC_NO_DEPRECATE
#include "nvidia.h"
#include "Types.h"
#include "Helpers.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace Types;

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*


//"Card" MACRO_CONCAT("", __COUNTER__)

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be 
//easy to locate their place - perhaps some option added to the input manager


*/
//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX(int x, int y) {return ((x > y) ? x : y);}

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
struct ContWrapper;
class CardContainer;

bool AddObjectToArray(vector<GameObject*> &objectArray, GameObject &object);
template<typename T, typename K>
bool AddObjectToArray(vector<T> &objectArray, K &object, int beginPos, int endPos);
template<>
bool AddObjectToArray<ContWrapper, GameObject>(vector< ContWrapper > &objectArray, GameObject &object, int beginPos, int endPos);
template<>
bool AddObjectToArray<ContWrapper, Container>(vector< ContWrapper > &objectArray, Container &object, int beginPos, int endPos);
bool ResetPositionInArray(vector<GameObject*> &objectArray, GameObject &object, int newIndex);
template<typename T, typename K>
bool ResetPositionInArray(vector<T> &objectArray, K &object, int beginPos, int endPos, bool(*func)(vector<T> &objArray, K &obj));
GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int order);

class GameObject {
public:
    SString name;
    int zIndex = -1;

    Color color = LIGHTGRAY;
    Rectangle position = {0,0,0,0};
    Texture2D texture = {0};

    bool concedeDrawing = false;// this is used by objects like containers to manage sub objects
    bool isActive = true;

    GameObject() = default;

    GameObject(GameObject &obj) = default;

    GameObject(GameObject &&obj) noexcept(false) {};

    explicit GameObject(SString &name) : name(name) {};

    GameObject& operator=(const GameObject &obj) = default;

    GameObject& operator=(GameObject &&obj) noexcept(false) { *this = obj; return *this; };

    virtual void Draw() {};

    virtual ~GameObject() = default;
};

class Board : public GameObject {
public:

    Board() {
        
    }

    Board(SString &name) : GameObject(name){
        
    }

    void Draw() override { 
        DrawRectangleRec(position, color); 
    };
};

class Card : public GameObject {
public:

    Card() {

    }

    Card(SString &name, Rectangle rectangle, Texture2D image) : GameObject(name) {
        position = rectangle;
        texture = image;
    }

    //ABILITY
    ///maybe add a function pointer for each
    ///or an event type

    void Draw() override {
        DrawRectangleRec(position, color);
    }
};

enum ContainerType {
    LOGICAL,// don't show object or his children
    WRAPPER,// shows only his children 
    OVERLAY,// show only object 
    MATERIAL// show object and his children
};

class Container : public GameObject {
public:

    Container() = default;

    explicit Container(SString &name) : GameObject(name) {};

    vector< ContWrapper > children;

    ContainerType type = LOGICAL;

    ContWrapper& operator[] (int n) { return children[n]; }

    virtual void Draw() override {}
    virtual void AddChild(Container* obj) {

        AddObjectToArray<ContWrapper, GameObject>(children, *obj, 0, children.size() - 1);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }
    virtual void AddChild(GameObject* obj) {

        AddObjectToArray<ContWrapper, GameObject>(children, *obj, 0, children.size() - 1);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }

    friend GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int order);
};

struct ContWrapper
{
    union
    {
        GameObject* go_pointer = nullptr;
        Container* c_pointer;
    };
    
    char index = -1;

    ContWrapper(GameObject* pointer)
    {
        go_pointer = pointer;
        index = 0;
    }

    ContWrapper(Container* pointer)
    {
        c_pointer = pointer;
        index = 1;
    }

    ContWrapper& operator=(GameObject* pointer)
    {
        go_pointer = pointer;
        index = 0;
    }

    ContWrapper& operator=(Container* pointer)
    {
        c_pointer = pointer;
        index = 1;
    }

    bool operator==(ContWrapper& other)
    {
        return go_pointer == other.go_pointer;
    }
};

class CardContainer : public Container {
public:

    CardContainer() = default;

    explicit CardContainer(SString &name) : Container(name) {};

    static const vector<Card*> ExtractNCardsFrom(vector<Card*>& container, int n);

    void Draw() override {

        //draws himself first,
        if (type == OVERLAY || type == MATERIAL)
            DrawRectangleRec(position, color);

        //then the children
        if (type == WRAPPER || type == MATERIAL)
        for (auto card: children){
            auto genericObj = card.go_pointer;
            genericObj->Draw();
        }
    }

    void AddList(vector<Card*> const &cards){
        for (auto card : cards) {
            AddChild(card);
        }
    }

    virtual void AddChild(Card* obj) {

        AddObjectToArray<ContWrapper, GameObject>(children, *static_cast<GameObject*>(obj), 0 ,children.size() - 1);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }
};

//used for random extracting or shuffling
const vector<Card*> CardContainer::ExtractNCardsFrom(vector<Card*>& container, int n)
{
    vector<Card*> selected;
    vector<int> selectedIndexes;

    if (n > container.size())
        return selected;

    for (int i = n; n >= 1;) {
        int randomN = GetRandomValue(0,container.size()-1);

        if ([&]()->bool {for (auto idx : selectedIndexes) { if (idx == randomN) return false; } return true; }()) {
            selectedIndexes.push_back(randomN);
            n--;
        }
    }

    for (auto it : selectedIndexes) {
        selected.push_back(container[it]);
    }

    return selected;
}

GameObject* GetObjectUnderPoint(Vector2 point, vector<GameObject*> &objectArray, int& order) {
    auto it = objectArray.end();
    auto const begin = objectArray.begin();
    --it;
    while ( it != begin) {
        if ((*it)->isActive && CheckCollisionPointRec(point, (*it)->position))
        {
            --order;
        }
        if (order < 0)
            break;
        --it;
    }

    if (order >= 0 || it == begin && (**it).zIndex == -1)
        return nullptr;
    return *it;
}

GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int order) {
    
    GameObject* returnPtr = nullptr;

    reverse(container.children.begin(), container.children.end());
    for (auto variant_child : container.children) {
        if (variant_child.index == 0)//GameObject
        {
            const auto ptr = variant_child.go_pointer;
            if (CheckCollisionPointRec(point, ptr->position))
            {
                if (ptr) {
                    if (ptr->zIndex != -1)
                    {
                        returnPtr = ptr;
                        break;
                    }

                    returnPtr = nullptr;
                    break;
                }
            }
        }
        else {//Container
            const auto ptr = GetObjectUnderPoint(point, *(variant_child.c_pointer), order);
            if (ptr)
            {
                if (ptr->zIndex != -1)
                {
                    returnPtr = ptr;
                    break;
                }

                returnPtr = nullptr;
                break;
            }
        }
    }
    reverse(container.children.begin(), container.children.end());

    return returnPtr;
}

bool AddObjectToArray(vector<GameObject*> &objectArray, GameObject &object) {
    auto iterator = (objectArray.size()) ? objectArray.end() - 1 : objectArray.end();
    while (iterator != objectArray.begin() && object.zIndex < (*(*(iterator))).zIndex) {
        --iterator;
    }
    if (iterator == objectArray.begin() && object.zIndex >= (*(*(iterator))).zIndex) {
        ++iterator;
    }

    objectArray.insert(iterator, &object);

    return true;
}

//Begin is the first element that will be compared from the list and endpos is the last
template<typename T, typename K>
bool AddObjectToArray(vector<T> &objectArray, K &object, int beginPos, int endPos) {//trebuie retestata

    int idx = beginPos;

    auto iterator = objectArray.begin();

    //auto begin = objectArray.begin();
    //auto iterator = begin + (endPos - 1);
    //auto start = begin + beginPos;

    while (idx <= endPos && object.zIndex >= (*(iterator + idx))->zIndex) {
        ++idx;
    }

    objectArray.insert(iterator + idx, &object);

    return true;//nu merge ordonarea
}

template<>
bool AddObjectToArray<ContWrapper, GameObject>(vector< ContWrapper > &objectArray, GameObject &object, int beginPos, int endPos) {
    auto iterator = (objectArray.size()) ? objectArray.end() - 1 : objectArray.begin();
    GameObject* pointer = nullptr;

    reverse(objectArray.begin(), objectArray.end());

    for (auto obj : objectArray)
    {
        pointer = obj.go_pointer;

        if (obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

    reverse(objectArray.begin(), objectArray.end());

    ContWrapper obj = &object;
    objectArray.insert(iterator, obj);

    return true;
}

template<>
bool AddObjectToArray<ContWrapper, Container>(vector< ContWrapper > &objectArray, Container &object, int beginPos, int endPos) {
    auto iterator = (objectArray.size()) ? objectArray.end() - 1 : objectArray.begin();
    GameObject* pointer = nullptr;

    reverse(objectArray.begin(), objectArray.end());

    for (auto obj : objectArray)
    {
        pointer = obj.go_pointer;

        if (obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

    reverse(objectArray.begin(), objectArray.end());

    ContWrapper obj = &object;
    objectArray.insert(iterator, obj);

    return true;
}

bool ResetPositionInArray(vector<GameObject*> &objectArray, GameObject &object, int newIndex) {
    for (auto it = objectArray.begin(); it != objectArray.end(); ++it) {
        if ((*it) == &object) {
            objectArray.erase(it);
            break;
        }
    }

    if (newIndex == ABSOLUT_NEW_INDEX)
        object.zIndex = objectArray[objectArray.size()-1]->zIndex + 1;
    else if (newIndex != INVALID_NEW_INDEX)
        object.zIndex = newIndex;

    return AddObjectToArray(objectArray, object);
}

//This function requires the index of the first element to be compared to, the last element, if the new Obj index is absolute or a new index
template<typename K>
bool ResetPositionInArray(vector<ContWrapper> &objectArray, K &object, int beginPos , int endPos , bool (*func)(vector<ContWrapper> &objArray, K &obj)) {
    auto begin = objectArray.begin();

    int idx = beginPos;
    for (; idx != endPos + 1; ++idx) {
        if ((*(begin + idx)).go_pointer == &object) {
            objectArray.erase(begin + idx);
            break;
        }
    }

    func(objectArray , object);

    if (idx < beginPos)
    {
        beginPos--; endPos--;
    }
    else if (idx < endPos)
        endPos--;

    return AddObjectToArray<ContWrapper, K>(objectArray, object, beginPos, endPos);
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
    
    Container activeObjects;

    vector<Card*> cardDatabase;

    SString name("The First Board");
    Board board;
    board.name = name;
    board.position = { 0, 0, screenWidth, screenHeight };
    board.zIndex = -1;
    activeObjects.AddChild(&board);

    SString numeCarte("Carte");
    Texture2D texture = { 0 };
    Card card(numeCarte, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture);
    card.zIndex = 1;
    card.color = GREEN;

    SString numeCarte1("Carte1");
    Texture2D texture1 = { 0 };
    Card card1(numeCarte1, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture1);
    card1.zIndex = 2;
    card1.color = BLUE;

    SString numeCarte2("Carte2");
    Texture2D texture2 = { 0 };
    Card card2(numeCarte2, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture2);
    card2.zIndex = 2;
    card2.color = RED;

    SString numeCarte3("Carte3");
    Texture2D texture3 = { 0 };
    Card card3(numeCarte3, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture3);
    card3.zIndex = 4;
    card3.color = BLACK;

    SString numeCarte4("Carte4");
    Texture2D texture4 = { 0 };
    Card card4(numeCarte4, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture4);
    card4.zIndex = 4;
    card4.color = PINK;

    /*Board board1;
    board1.zIndex = 2;
    Board board2;
    board2.zIndex = 4;
    Board board3(name.Substitute("Board3"));
    board3.zIndex = 4;
    Board board4;
    board4.zIndex = 4;
    Board board5(name.Substitute("Board5"));
    board5.zIndex = 4;

    AddObjectToArray(activeObjects, board1);
    AddObjectToArray(activeObjects, board2);
    AddObjectToArray(activeObjects, board3);
    AddObjectToArray(activeObjects, board4);
    AddObjectToArray(activeObjects, board5);

    ResetPositionInArray(activeObjects, board3, ABSOLUT_NEW_INDEX);*/

    //////////////
    cardDatabase.emplace_back(&card);
    cardDatabase.emplace_back(&card1);
    cardDatabase.emplace_back(&card2);
    cardDatabase.emplace_back(&card3);
    cardDatabase.emplace_back(&card4);

    /*-----[GAME]-----------------------------------------------------------------------------------------------------------------------------*/

    SString hand_name("Hand");
    CardContainer hand(hand_name);
    auto draw = CardContainer::ExtractNCardsFrom(cardDatabase, 3);
    hand.AddList(draw);
    hand.type = WRAPPER;

    for (auto _card : cardDatabase) {
        _card->concedeDrawing = true;
        _card->isActive = false;
    }
    //so every card becomes inactive and a child
    for (auto _card : hand.children) {
        GameObject* pointer = nullptr;
        if (!_card.index)
            pointer = _card.go_pointer;
        if (pointer)
            pointer->isActive = true;
    }
    //and every child in hand is now active
    AddObjectToArray<ContWrapper, Container>(activeObjects.children,*(static_cast<Container*>(&hand)), 0, activeObjects.children.size() - 1);//this should be a gamemanager object function

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
            dragSelectedObject = GetObjectUnderPoint(mouse, activeObjects, 0);

            if (dragSelectedObject == nullptr)
                break;

            //set focus - trebuie facuta treaba asta mai organizat, managed by input manager
            if (dragSelectedObject != nullptr && dragSelectedObject != hand.children[hand.children.empty() ? 0 : hand.children.size() - 1].go_pointer)
            {
                ResetPositionInArray<GameObject>(
                    hand.children, 
                    *dragSelectedObject, 
                    0,
                    hand.children.size(),
                    [](vector<ContWrapper> &objArray, GameObject &obj)->bool
                {
                    obj.zIndex = (objArray[objArray.size() - 1].go_pointer)->zIndex + 1;
                    return true;
                });
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
            if (dragSelectedObject != GetObjectUnderPoint(mouse, activeObjects, 0))
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

        for (auto obj : activeObjects.children){

            if (!obj.index) {
                auto pointer = obj.go_pointer;
                pointer->Draw();
            } else {
                auto pointer = obj.c_pointer;
                pointer->Draw();
            }
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

