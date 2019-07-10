#define _CRT_NONSTDC_NO_DEPRECATE
#include "nvidia.h"
#include "SString.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include <variant>

using namespace std;

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*


//"Card" MACRO_CONCAT("", __COUNTER__)

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be 
//easy to locate their place - perhaps some option added to the input manager


*/
//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX(x,y) ((x > y) ? x : y )

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
GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int order);

class GameObject {
public:
    SString::SString name;

    Rectangle position = {0,0,0,0};
    Texture2D texture = {0};
    Color color = LIGHTGRAY;
    int zIndex = -1;

    bool concedeDrawing = false;// this is used by objects like containers to manage sub objects
    bool isActive = true;

    GameObject() = default;

    GameObject(GameObject &obj) = default;

    GameObject(GameObject &&obj) noexcept(false) {};

    explicit GameObject(SString::SString &name) : name(name) {};

    GameObject& operator=(const GameObject &obj) = default;

    GameObject& operator=(GameObject &&obj) noexcept(false) { *this = obj; return *this; };

    virtual void Draw() {};

    virtual ~GameObject() = default;
};

class Board : public GameObject {
public:

    Board() {
        
    }

    Board(SString::SString &name) : GameObject(name){
        
    }

    void Draw() override { 
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
    vector< variant<GameObject*,Container*> > children;

    bool isZAranged = false;

    ContainerType type = ContainerType::LOGICAL;

    auto operator[] (int n) { return children[n]; }

    virtual void Draw() override {}
    virtual void AddChild(Container* obj) {

        const variant<GameObject*, Container*> var = obj;

        children.push_back(var);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }
    virtual void AddChild(GameObject* obj) {

        const variant<GameObject*, Container*> var = obj;

        children.push_back(var);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }

    friend GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int order);
};

class CardContainer : public Container {//perhaps cardcontainer might inherit from Container, a class that will be used mostly for the UI, but cardDB will have isMaterial = false, so it wont be shown
public:
    vector<Card*> cards;//cards should become private

    CardContainer() = default;

    static const vector<Card*> ExtractNCardsFrom(vector<Card*>& container, int n);

    bool Arange() {;

        int start = 0;
        int end = cards.size() - 1;
        for (auto obj : cards) {
            ResetPositionInArray<Card>(cards, *obj, start, end);
            start++;
        }

        return true;
    }

    void Draw() override {

        if (!isZAranged)
        {
            isZAranged = Arange();

            if (!isZAranged)
                return;
        }

        //draws himself first,
        if (type == ContainerType::OVERLAY || type == ContainerType::MATERIAL)
            DrawRectangleRec(position, color);

        //then the children
        if (type == ContainerType::WRAPPER || type == ContainerType::MATERIAL)
        for (auto card: cards){
            card->Draw();
        }
    }

    void AddList(vector<Card*> const &cards){
        for (auto card : cards) {
            AddChild(card);
        }
    }

    virtual void AddChild(Card* obj) {
        cards.push_back(obj);

        this->zIndex = MAX(this->zIndex, obj->zIndex);
    }
};

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

GameObject* GetObjectUnderPoint(Vector2 point, vector<GameObject*> &objectArray, int& order) {// e o problema aici, active Objects si Hand nu au aceeasi ordine
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
    
    reverse(container.children.begin(), container.children.end());//iterating from the last to the first - this needs testing
    for (auto variant_child : container.children) {
        if (variant_child.index() == 0)//GameObject
            CheckCollisionPointRec(point, get<GameObject*>(variant_child)->position);
        else {//Container
            const auto ptr = GetObjectUnderPoint(point, *get<Container*>(variant_child), order);
            if (ptr)
                return nullptr;
        }
    }
    reverse(container.children.begin(), container.children.end());//making changes back
}

bool AddObjectToArray(vector<GameObject*> &objectArray, GameObject &object) {
    auto iterator = objectArray.end() - 1;
    while (iterator != objectArray.begin() && object.zIndex <= (*(*(iterator))).zIndex) {
        --iterator;
    }
    if (iterator == objectArray.begin() && object.zIndex > (*(*(iterator))).zIndex) {
        ++iterator;
    }

    objectArray.insert(iterator, &object);

    return true;
}

bool AddObjectToArray(vector< variant<GameObject*, Container*> > &objectArray, GameObject &object) {
    auto iterator = objectArray.end() - 1;
    GameObject* pointer = nullptr;

    while (iterator != objectArray.begin()){

        if(!(*iterator).index())//GameObject
            pointer = static_cast<GameObject*>(get<GameObject*>(*iterator));
        else
            pointer = static_cast<GameObject*>(get<Container*>(*iterator));

        if (object.zIndex <= pointer->zIndex)
            --iterator;
    }

    if (objectArray.size() == 1){
        if (!(*iterator).index())
            pointer = static_cast<GameObject*>(get<GameObject*>(*iterator));
        else
            pointer = static_cast<GameObject*>(get<Container*>(*iterator));

        if (object.zIndex > pointer->zIndex)
            ++iterator;
    }
    else if (iterator == objectArray.begin() && object.zIndex > pointer->zIndex) {
        ++iterator;
    }

    variant<GameObject*, Container*> obj = &object;
    objectArray.insert(iterator, obj);

    return true;
}

bool AddObjectToArray(vector< variant<GameObject*, Container*> > &objectArray, Container &object) {
    auto iterator = objectArray.end() - 1;
    GameObject* pointer = nullptr;

    while (iterator != objectArray.begin()) {

        if (!(*iterator).index())//GameObject
            pointer = static_cast<GameObject*>(get<GameObject*>(*iterator));
        else
            pointer = static_cast<GameObject*>(get<Container*>(*iterator));

        if (object.zIndex <= pointer->zIndex)
            --iterator;
    }

    if (objectArray.size() == 1) {
        if (!(*iterator).index())
            pointer = static_cast<GameObject*>(get<GameObject*>(*iterator));
        else
            pointer = static_cast<GameObject*>(get<Container*>(*iterator));

        if (object.zIndex > pointer->zIndex)
            ++iterator;
    }
    else if (iterator == objectArray.begin() && object.zIndex > pointer->zIndex) {
        ++iterator;
    }

    variant<GameObject*, Container*> obj = &object;
    objectArray.insert(iterator, obj);

    return true;
}

template<typename T>
bool AddObjectToArray(vector<T*> &objectArray, T &object, int beginPos, int endPos) {//trebuie retestata

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
        object.zIndex = objectArray[objectArray.size() - 1]->zIndex + 1;
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
    
    Container activeObjects;

    CardContainer cardDatabase;

    SString::SString name("The First Board");
    Board board;
    board.name = name;
    board.position = { 0, 0, screenWidth, screenHeight };
    board.zIndex = -1;
    activeObjects.AddChild(&board);

    SString::SString numeCarte("Carte");
    Texture2D texture = { 0 };
    Card card(numeCarte, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture);
    card.zIndex = 1;
    card.color = GREEN;
    //AddObjectToArray(activeObjects, card);

    SString::SString numeCarte1("Carte1");
    Texture2D texture1 = { 0 };
    Card card1(numeCarte1, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture1);
    card1.zIndex = 2;
    card1.color = BLUE;
    //AddObjectToArray(activeObjects, card1);

    SString::SString numeCarte2("Carte2");
    Texture2D texture2 = { 0 };
    Card card2(numeCarte2, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture2);
    card2.zIndex = 2;
    card2.color = RED;
    //AddObjectToArray(activeObjects, card2);

    SString::SString numeCarte3("Carte3");
    Texture2D texture3 = { 0 };
    Card card3(numeCarte3, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture3);
    card3.zIndex = 4;
    card3.color = BLACK;
    //AddObjectToArray(activeObjects, card3);

    SString::SString numeCarte4("Carte4");
    Texture2D texture4 = { 0 };
    Card card4(numeCarte4, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture4);
    card4.zIndex = 4;
    card4.color = PINK;
    //AddObjectToArray(activeObjects, card4);

    Board board1;
    board1.zIndex = 2;
    Board board2;
    board2.zIndex = 4;
    Board board3(name.Substitute("Board3"));
    board3.zIndex = 4;
    Board board4;
    board4.zIndex = 4;
    Board board5(name.Substitute("Board5"));
    board5.zIndex = 4;

    /*AddObjectToArray(activeObjects, board1);
    AddObjectToArray(activeObjects, board2);
    AddObjectToArray(activeObjects, board3);
    AddObjectToArray(activeObjects, board4);
    AddObjectToArray(activeObjects, board5);

    ResetPositionInArray(activeObjects, board3, ABSOLUT_NEW_INDEX);*/

    //////////////
    cardDatabase.cards.push_back(&card);
    cardDatabase.cards.push_back(&card1);
    cardDatabase.cards.push_back(&card2);
    cardDatabase.cards.push_back(&card3);
    cardDatabase.cards.push_back(&card4);

    /*-----[GAME]-----------------------------------------------------------------------------------------------------------------------------*/

    CardContainer hand;
    hand.AddList(CardContainer::ExtractNCardsFrom(cardDatabase.cards, 3));
    hand.type = WRAPPER;

    for (auto _card : cardDatabase.cards) {
        _card->concedeDrawing = true;
        _card->isActive = false;
    }
    //so every card becomes inactive and a child
    for (auto _card : hand.cards) {
        _card->isActive = true;
    }
    //and every child in hand is now active
    AddObjectToArray(activeObjects.children,*((Container*)&hand));//this should be a gamemanager object function

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

        for (auto obj : activeObjects.children){

            if (!obj.index()) {
                auto pointer = get<GameObject*>(obj);
                pointer->Draw();
            } else {
                auto pointer = get<Container*>(obj);
                pointer->Draw();
            }
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

