#pragma once
#include "nvidia.h"
#include "Types.h"
#include "Helpers.h"
#include "raylib.h"
#include "externalFunction.h"
#include <iostream>
#include <vector>
#include <memory.h>

using namespace std;
using namespace Types;

//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX(int x, int y) { return ((x > y) ? x : y); }

#define GET_NAME(x) #x //could be used in combination with SClose for printing variables after destruction of SClose object

#define CONCAT_NUM( x, y ) x#y
#define MACRO_CONCAT( x, y ) CONCAT_NUM( x, y )

#define iff while
typedef Vector2 Point;

#define INVALID_NEW_INDEX -1
#define ABSOLUT_NEW_INDEX -2

//-----[Classes]---------------------------------------------------------------------------------------------------------------------------------------------------------------

class GameObject;
class Card;
class Board;
class Container;
struct Owner;
class CardContainer;

bool                            AddObjectToArray(vector<GameObject*> &objectArray, GameObject &object);
template<typename T, typename K>
bool                            AddObjectToArray(vector<T> &objectArray, K &object, int beginPos, int endPos, void* parent);
template<>
bool                            AddObjectToArray<Owner, GameObject>(vector< Owner > &objectArray, GameObject &object, int beginPos, int endPos, void* parent);
template<>
bool                            AddObjectToArray<Owner, Container>(vector< Owner > &objectArray, Container &object, int beginPos, int endPos, void* parent);
bool                            ResetPositionInArray(vector<GameObject*> &objectArray, GameObject &object, int newIndex);
GameObject*                     GetGameObjectUnderPoint(Vector2 point, Container& container, int& order);

class GameObject {
public:
    SString                     name;
    int                         zIndex = -1;

    Color                       color = LIGHTGRAY;
    Rectangle                   position = { 0,0,0,0 };
    Texture2D                   texture = { 0 };

    bool                        concedeDrawing = false;// this is used by objects like containers to manage sub objects
    bool                        isActive = true;

    GameObject()                = default;

    explicit GameObject(SString name, Rectangle pos) : name(name), position(pos) {};

    GameObject(GameObject &obj);

    GameObject&                 operator=(const GameObject &obj) = default;

    virtual GameObject&         GetCopy();

    virtual void Draw()         {};

    virtual ~GameObject()       = default;
};
GameObject::GameObject(GameObject &obj)
{
    *this = obj; 
    this->name = obj.name;
}
GameObject& GameObject::GetCopy()
{
    GameObject go;
    go = *this;
    go.name = this->name;
    return go;
}

class Board : public GameObject {
public:

    Board()                     {}

    Board(SString name, Rectangle pos) : GameObject(name, pos) {}

    void Draw()                 override;
};
void Board::Draw()
{
    DrawRectangleRec(position, color);
};

class Card : public GameObject {
public:

    Card()                      {}

    Card(SString name, Rectangle rectangle, Texture2D image) : GameObject(name, rectangle) {
        texture = image;
    }

    //ABILITY
    ///maybe add a function pointer for each
    ///or an event type

    void Draw()                 override;
};
void Card::Draw()
{
    DrawRectangleRec(position, color);
};

struct Owner
{
    union
    {
        shared_ptr<GameObject>            go_pointer;
        shared_ptr<Container>             c_pointer;
    };

    char                        index = -1;

    Container*                  parent = nullptr;

    Owner()                     {};
    Owner(GameObject* pointer);
    Owner(Container* pointer);
    Owner(const Owner& other){} //these two will create problems
    Owner(const Owner&& other);
    auto                        operator=(const Owner& other){} //these will create problems
    Owner&                      operator=(GameObject* pointer);
    Owner&                      operator=(Container* pointer);
    bool                        operator==(Owner& other);
    bool                        operator==(void* ptr);
    bool                        operator!=(void* ptr);
    GameObject*                 operator->();
    void                        MakeCopy(Owner& owner);
    void                        Destroy();

    ~Owner();
};
Owner::Owner(GameObject* pointer)
{
    go_pointer = make_shared<GameObject>(*pointer);
    index = 0;
}
Owner::Owner(const Owner&& other)
{
    this->go_pointer.reset(other.go_pointer.get());
    this->index = other.index;
}
Owner& Owner::operator=(GameObject* pointer)
{
    go_pointer = make_shared<GameObject>(*pointer);
    index = 0;
    return *this;
}
bool Owner::operator==(Owner& other)
{
    return go_pointer == other.go_pointer;
}
bool Owner::operator==(void* ptr)
{
    return go_pointer.get() == ptr;
}
bool Owner::operator!=(void* ptr)
{
    return !(go_pointer.get() == ptr);
}
GameObject* Owner::operator->()
{
    return go_pointer.get();
}
Owner::~Owner() {
    Destroy();
}

class Container : public GameObject {
public:
    enum ContainerType {
        LOGICAL,// don't show object or his children
        WRAPPER,// shows only his children 
        OVERLAY,// show only object 
        MATERIAL// show object and his children
    };

    vector< Owner >             children;

    ContainerType               type = LOGICAL;

    Container()                 {};

    explicit Container(SString name, Rectangle pos) : GameObject(name,pos) {};
    Container&                  GetCopy();
    Container&                  operator=(Container& cont);
    Owner&                      operator[](int n);
    void                        Draw();
    virtual void                AddChild(Container* obj);
    virtual void                AddChild(GameObject* obj);
    virtual void                Destroy();

    ~Container();

    friend GameObject*          GetGameObjectUnderPoint(Vector2 point, Container& container, int& order);
};
Container& Container::operator= (Container& cont)
{
    this->type = cont.type;
    this->children = cont.children;
    return *this;
}
Container& Container::GetCopy()
{
    Container cont;
    cont.type = type;
    for (auto obj : children)
    {
        Owner* newOwner = new Owner;
        newOwner->MakeCopy(obj);
        cont.children.emplace_back(newOwner);
    }
    return cont;
}
Owner& Container::operator[] (int n)
{
    return children[n];
}
void Container::Draw() {}
void Container::AddChild(Container* obj) {

    AddObjectToArray<Owner, GameObject>(children, *obj, 0, children.size() - 1, this);

    this->zIndex = MAX(this->zIndex, obj->zIndex);
}
void Container::AddChild(GameObject* obj) {

    AddObjectToArray<Owner, GameObject>(children, *obj, 0, children.size() - 1, this);

    this->zIndex = MAX(this->zIndex, obj->zIndex);
}
void Container::Destroy()
{
    for (Owner obj : children)
    {
        obj.Destroy();
    }
}
Container::~Container()
{
    Destroy();
}

//From Owner
void Owner::MakeCopy(Owner& owner)
{
    if (owner.index == 0)
    {
        GameObject* ptr = new GameObject;
        *ptr = (*owner.go_pointer).GetCopy();
    }
    else
    {
        Container* ptr = new Container;
        *ptr = (*owner.c_pointer).GetCopy();
    }
}
void Owner::Destroy()
{
    index = -1;
    if (index == 0)
        go_pointer.reset();
    else
        c_pointer.reset();
}
Owner::Owner(Container* pointer)
{
    c_pointer = make_shared<Container>(*pointer);
    index = 1;
}
Owner& Owner::operator=(Container* pointer)
{
    c_pointer = make_shared<Container>(*pointer);
    index = 1;
    return *this;
}

class CardContainer : public Container {
public:

    CardContainer()             = default;

    explicit CardContainer(SString name, Rectangle pos) : Container(name, pos) {};

    void                        Draw();
    void                        AddList(vector<Card*> const& cards);
    virtual void                AddChild(Card* obj);

    static vector<Card*>  ExtractNCardsFrom(vector<Card*>& container, int n);
};
void CardContainer::Draw() {

    //draws himself first,
    if (type == OVERLAY || type == MATERIAL)
        DrawRectangleRec(position, color);

    //then the children
    if (type == WRAPPER || type == MATERIAL)
        for (auto card : children) {
            auto genericObj = card.go_pointer;
            genericObj->Draw();
        }
}
void CardContainer::AddList(vector<Card*> const &cards) {
    for (auto card : cards) {
        AddChild(card);
    }
}
void CardContainer::AddChild(Card* obj) {

    AddObjectToArray<Owner, GameObject>(children, *static_cast<GameObject*>(obj), 0, children.size() - 1, this);

    this->zIndex = MAX(this->zIndex, obj->zIndex);
}

class HorizontalContainer : public Container
{
public:
    enum AllocateType
    {
        GET_FIRST_AVAILABLE,
        GET_LAST_AVAILABLE
    };

    AllocateType                allocateType = GET_FIRST_AVAILABLE;

    int                         numOfColumns = 1;
    int                         numOfLines = 1;

    float                       marginLeft = 0;
    float                       marginUp = 0;
    float                       marginRight = 0;
    float                       marginDown = 0;

    float                       spaceBetween = 0;

    bool                        stretchEnabled = false;

    Rectangle*                  positionTable = nullptr;
    Rectangle*                  savedPositionTable = nullptr;
    int*                        indexTable = nullptr;
    int*                        optimizeIndexTable = nullptr;
    bool                        overwritePosOn = false;

    explicit HorizontalContainer(SString name, Rectangle pos, int columns, int lines);
    explicit HorizontalContainer(int columns, int lines);
    explicit HorizontalContainer(SString name, Rectangle pos, int columns, int lines, float left, float up, float right, float down, float space);
    explicit HorizontalContainer(int columns, int lines, float left, float up, float right, float down, float space);

    void InitSize();
    int AssignPos();
    void OverwritePos();
    void Draw();
    void AddChild(Container* obj);
    void AddChild(GameObject* obj);
    ~HorizontalContainer();
};
HorizontalContainer::HorizontalContainer(SString name, Rectangle pos, int columns, int lines) :
    Container(name, pos), numOfColumns(columns), numOfLines(lines)
{
    color = PURPLE;
    InitSize();
};
HorizontalContainer::HorizontalContainer(int columns, int lines) :
    numOfColumns(columns), numOfLines(lines)
{
    color = PURPLE;
    InitSize();
};
HorizontalContainer::HorizontalContainer(SString name, Rectangle pos, int columns, int lines, float left, float up, float right, float down, float space) :
    Container(name, pos), numOfColumns(columns), numOfLines(lines),
    marginLeft(left), marginUp(up), marginRight(right), marginDown(down), spaceBetween(space)
{
    color = PURPLE;
    InitSize();
};
HorizontalContainer::HorizontalContainer(int columns, int lines, float left, float up, float right, float down, float space) :
    numOfColumns(columns), numOfLines(lines),
    marginLeft(left), marginUp(up), marginRight(right), marginDown(down), spaceBetween(space)
{
    color = PURPLE;
    InitSize();
};
void HorizontalContainer::InitSize()
{
    positionTable = (Rectangle*)calloc(numOfColumns*numOfLines, sizeof(Rectangle));

    const auto totalHeight = position.height - marginUp - marginDown - spaceBetween * (static_cast<float>(numOfLines) - 1.0f);
    const auto totalWidth = position.width - marginLeft - marginRight - spaceBetween * (static_cast<float>(numOfColumns) - 1.0f);

    const auto width = totalWidth / static_cast<float>(numOfColumns);
    const auto height = totalHeight / static_cast<float>(numOfLines);

    for (int lin = 0; lin < numOfLines; lin++)
        for (int col = 0; col < numOfColumns; col++)
        {//CAUTION: turning corner position into center positions
            float y = position.y + marginUp + spaceBetween * static_cast<float>(lin) + height * static_cast<float>(lin) + height / 2;
            float x = position.x + marginLeft + spaceBetween * static_cast<float>(col) + width * static_cast<float>(col) + width / 2;

            const Rectangle aux{ x,y,width,height };
            positionTable[numOfLines * lin + col] = aux;
        }

    indexTable = (int*)calloc(numOfColumns*numOfLines, sizeof(int));
    optimizeIndexTable = (int*)calloc(numOfColumns*numOfLines, sizeof(int));
    savedPositionTable = (Rectangle*)calloc(numOfColumns*numOfLines, sizeof(Rectangle));
}
int HorizontalContainer::AssignPos()
{
    switch (allocateType)
    {
    case GET_LAST_AVAILABLE:
        for (int idx = numOfColumns * numOfLines - 1; idx >= 0; idx--)
            if (optimizeIndexTable[idx] == 0)
            {
                optimizeIndexTable[idx] = 1;
                return idx;
            }
        return -1;
    case GET_FIRST_AVAILABLE:
    default:
        for (int idx = 0; idx < numOfColumns*numOfLines; idx++)
            if (optimizeIndexTable[idx] == 0)
            {
                optimizeIndexTable[idx] = 1;
                return idx;
            }
        return -1;
    }
}
void HorizontalContainer::OverwritePos()
{
    int index = 0;
    for (auto obj : children) {
        auto genericObj = obj.go_pointer;

        savedPositionTable[indexTable[index]] = genericObj->position;
        auto getPos = positionTable[indexTable[index]];

        if (stretchEnabled)
        {
            getPos.x -= getPos.width / 2;
            getPos.y -= getPos.height / 2;
        }
        else
        {
            getPos.width = genericObj->position.width;
            getPos.height = genericObj->position.height;
            getPos.x -= getPos.width / 2;
            getPos.y -= getPos.height / 2;
        }

        genericObj->position = getPos;
        genericObj->Draw();
        index++;
    }
}
void HorizontalContainer::Draw() 
{
    if (!overwritePosOn)
    {
        OverwritePos();
        overwritePosOn = true;
    }

    if (type == OVERLAY || type == MATERIAL)
        DrawRectangleRec(position, color);

    if (type == WRAPPER || type == MATERIAL)
        for (auto obj : children) {
            auto genericObj = obj.go_pointer;
            genericObj->Draw();
        }
}
void HorizontalContainer::AddChild(Container* obj) 
{

    AddObjectToArray<Owner, GameObject>(children, *obj, 0, children.size() - 1, this);

    this->zIndex = MAX(this->zIndex, obj->zIndex);

    indexTable[children.empty() ? 0 : children.size() - 1] = AssignPos();

    overwritePosOn = false;
}
void HorizontalContainer::AddChild(GameObject* obj) {

    AddObjectToArray<Owner, GameObject>(children, *obj, 0, children.size() - 1, this);

    this->zIndex = MAX(this->zIndex, obj->zIndex);

    indexTable[children.empty() ? 0 : children.size() - 1] = AssignPos();

    overwritePosOn = false;
}
HorizontalContainer::~HorizontalContainer()
{
    free(positionTable);
    free(indexTable);
    free(optimizeIndexTable);
    free(savedPositionTable);
}

//used for random extracting or shuffling
vector<Card*> CardContainer::ExtractNCardsFrom(vector<Card*>& container, int n)
{
    vector<Card*> selected;
    vector<int> selectedIndexes;

    if (n > container.size())
        return selected;

    for (int i = n; n >= 1;) {
        int randomN = GetRandomValue(0, container.size() - 1);

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
GameObject* GetGameObjectUnderPoint(Vector2 point, vector<GameObject*> &objectArray, int& order) {
    auto it = objectArray.end();
    auto const begin = objectArray.begin();
    --it;
    while (it != begin) {
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
GameObject* GetGameObjectUnderPoint(Vector2 point, Container& container, int& order) {//add bool LookForParent - returns only the object that you are on top and is also parent

    GameObject* returnPtr = nullptr;

    reverse(container.children.begin(), container.children.end());
    for (auto variant_child = container.children.begin(); variant_child != container.children.end() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 0)//GameObject
        {
            const auto ptr = (*variant_child).go_pointer;
            if (ptr && CheckCollisionPointRec(point, ptr->position))
            {

                if (ptr->zIndex != -1)
                {
                    returnPtr = ptr.get();
                    --order;
                    continue;
                }

                returnPtr = nullptr;
                --order;

            }
        }
        else {//Container
            const auto ptr = GetGameObjectUnderPoint(point, *((*variant_child).c_pointer), order);
            if (ptr) {
                if (ptr->zIndex != -1)
                {
                    returnPtr = ptr;
                    --order;
                    continue;
                }

                returnPtr = nullptr;
                --order;
            }
        }
    }
    reverse(container.children.begin(), container.children.end());

    return returnPtr;
}
//This function will return Container objects - if you specify true for getLogical it will also consider abstract containers
//You could combine GetParentUnderPoint and GetGameObjectUnderPoint to deduce if the object you are on has this parent,
//perhaps by iterating through numbers for order or somehow recursively reaching the parent
Container* GetParentUnderPoint(Vector2 point, Container& container, int& order, bool getLogical) {//add bool LookForParent - returns only the object that you are on top and is also parent

    Container* returnPtr = nullptr;

    reverse(container.children.begin(), container.children.end());
    for (auto variant_child = container.children.begin(); variant_child != container.children.end() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 1) {//Container

            if (getLogical || CheckCollisionPointRec(point, (*variant_child).c_pointer->position))
            {
                returnPtr = (*variant_child).c_pointer.get();
                --order;
            }

            if (order < 0)
                break;

            const auto ptr = GetParentUnderPoint(point, *((*variant_child).c_pointer), order, getLogical);

            returnPtr = ptr;

            if (order < 0)
                break;

        }
    }
    reverse(container.children.begin(), container.children.end());

    return returnPtr;
}
GameObject* GetObjectUnderPoint(Vector2 point, Container& container, int& order) {

    GameObject* returnPtr = nullptr;

    reverse(container.children.begin(), container.children.end());
    for (auto variant_child = container.children.begin(); variant_child != container.children.end() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 0)//GameObject
        {
            const auto ptr = (*variant_child).go_pointer;
            if (ptr && CheckCollisionPointRec(point, ptr->position))
            {

                if (ptr->zIndex != -1)
                {
                    returnPtr = ptr.get();
                    --order;
                    continue;
                }

                returnPtr = nullptr;
                --order;

            }
        }
        else {//Container
            const auto ptr1 = (*variant_child).c_pointer;
            if (ptr1 && CheckCollisionPointRec(point, ptr1->position))
            {
                returnPtr = ptr1.get();
                --order;
            }

            if (order < 0)
                break;

            const auto ptr2 = GetObjectUnderPoint(point, *((*variant_child).c_pointer), order);

            returnPtr = ptr2;

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
bool AddObjectToArray(vector<T> &objectArray, K &object, int beginPos, int endPos, void* parent = nullptr) {//trebuie retestata

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
bool AddObjectToArray<Owner, GameObject>(vector< Owner > &objectArray, GameObject &object, int beginPos, int endPos, void* parent) {
    auto iterator = (objectArray.size()) ? objectArray.end() - 1 : objectArray.begin();
    GameObject* pointer = nullptr;

    reverse(objectArray.begin(), objectArray.end());

    for (auto obj : objectArray)
    {
        pointer = obj.go_pointer.get();

        if (obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

    reverse(objectArray.begin(), objectArray.end());

    Owner* obj = new Owner(&object);
    obj->parent = static_cast<Container*>(parent);
    objectArray.insert(iterator, *obj);

    return true;
}
template<>
bool AddObjectToArray<Owner, Container>(vector< Owner > &objectArray, Container &object, int beginPos, int endPos, void* parent) {
    auto iterator = (objectArray.size()) ? objectArray.end() - 1 : objectArray.begin();
    GameObject* pointer = nullptr;

    reverse(objectArray.begin(), objectArray.end());

    for (auto obj : objectArray)
    {
        pointer = obj.go_pointer.get();

        if (obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

    reverse(objectArray.begin(), objectArray.end());

    Owner* obj = new Owner(&object);
    obj->parent = static_cast<Container*>(parent);
    objectArray.insert(iterator, *obj);

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
        object.zIndex = objectArray[objectArray.size() - 1]->zIndex + 1;
    else if (newIndex != INVALID_NEW_INDEX)
        object.zIndex = newIndex;

    return AddObjectToArray(objectArray, object, 0, objectArray.size() -1);
}
//This function requires the index of the first element to be compared to, the last element, if the new Obj index is absolute or a new index
template<typename K>
bool ResetPositionInArray(vector<Owner> &objectArray, K &object, int beginPos, int endPos, bool(*func)(vector<Owner> &objArray, K &obj), void* parent) {
    auto begin = objectArray.begin();

    int idx = beginPos;
    for (; idx != endPos + 1; ++idx) {
        if ((*(begin + idx)).go_pointer == &object) {
            objectArray.erase(begin + idx);
            break;
        }
    }

    func(objectArray, object);

    if (idx < beginPos)
    {
        beginPos--; endPos--;
    }
    else if (idx < endPos)
        endPos--;

    return AddObjectToArray<Owner, K>(objectArray, object, beginPos, endPos, parent);
}
//trebuie neaparat o functie de defragmentare