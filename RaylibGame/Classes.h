#pragma once
#include "raylib.h"
#include <vector>
#include <memory>
#include <variant>
#include <functional>
#include <iostream>
#include <stdlib.h>

//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX(int x, int y) { return ((x > y) ? x : y); }

#define SIMPLE(x) x
#define GET_NAME(x) #x //could be used in combination with SClose for printing variables after destruction of SClose object
#define CONCAT_NUM( x, y ) x#y
#define MACRO_CONCAT( x, y ) CONCAT_NUM( x, y )

#define IF while
#define BREAK_IF break;

#define CHECK(c, ret, msg) {if (!c) {cout<<msg; return ret;}};
#define CHECKRET(c, msg) {if (!c) {cout << msg; return;}}
#define CHECKBK(c, msg) {if (!c) {cout << msg; break;}}

#define INVALID_NEW_INDEX -1
#define ABSOLUT_NEW_INDEX -2

typedef void (draw)();

#define LAST_IDX(x) (x.children.empty() ? 0 : x.children.size() - 1)

//-----[Classes]---------------------------------------------------------------------------------------------------------------------------------------------------------------

class GameObject {
public:
    std::string                             name;
    int                                     zIndex = -1;

    Color                                   color = LIGHTGRAY;
    Rectangle                               position = { 0,0,0,0 };
    Texture2D                               texture = { 0 };

    bool                                    isActive = true;
    bool                                    isSelectable = false;

    GameObject(std::string str, int z, Color col, Rectangle pos);

    std::function<void()>                   draw;
};

class Board : public GameObject {
public:
    Board(std::string str, int z, Color col, Rectangle pos);
};

class Card : public GameObject {
public:
    Card(std::string str, int z, Color col, Rectangle pos);
};

class Container;

struct Owner
{
    std::variant<std::unique_ptr<GameObject>, std::unique_ptr<Container>>   pointer;

    Container*                                                              parent = nullptr;

    Owner() = default;
    Owner(const Owner& other);
    void operator=(const Owner& other);

    void SetGameObject(GameObject go);
    void SetContainer(Container c);
    int  GetZIndex();
    Rectangle GetPosition();
    bool GetIsActive();
    bool GetIsSelectable();
    void* GetPointer();
    std::function<void()> GetDraw();

    bool operator==(Owner& other);
    bool operator==(void* ptr);
    bool operator!=(Owner& other);
    bool operator!=(void* ptr);
};

class Container : public GameObject {
public:
    std::vector< Owner >                    children;

    enum ContainerType {
        LOGICAL,// don't show object or his children
        WRAPPER,// shows only his children 
        OVERLAY,// show only object 
        MATERIAL// show object and his children
    }                                       type = LOGICAL;

    Container(std::string str, int z, Color col, Rectangle pos, ContainerType t);

    virtual void                            PlaceChild(GameObject go, int beginPos, int endPos);
    virtual void                            PlaceChild(Container cont, int beginPos, int endPos);
    virtual void                            ResetChildPosition(Owner* child, int beginPos, int endPos, bool(*func)(Container &cont, Owner* obj));
    virtual Owner                           RemoveChild(int idx);//trebuie testata
    void                                    Destroy();

    virtual Owner*                          GetSelectableObjectUnderPoint(Vector2 point, int order);
    virtual Owner*                          GetGameObjectUnderPoint(Vector2 point, int order);
    virtual Owner*                          GetObjectUnderPoint(Vector2 point, int order);
    virtual Owner*                          GetParentUnderPoint(Vector2 point, int order, bool getLogical = false);

    Owner*                                  operator[] (int n);

};

class CardContainer {
public:
    std::vector<Card> cards;
};

class HorizontalContainer : public Container
{
public:
    enum AllocateType
    {
        GET_FIRST_AVAILABLE,
        GET_LAST_AVAILABLE
    };

    AllocateType                            allocateType = GET_FIRST_AVAILABLE;

    int                                     numOfColumns = 1;
    int                                     numOfLines = 1;

    float                                   marginLeft = 0;
    float                                   marginUp = 0;
    float                                   marginRight = 0;
    float                                   marginDown = 0;

    float                                   spaceBetween = 0;

    bool                                    stretchEnabled = false;

    Rectangle*                              positionTable = nullptr;
    Rectangle*                              savedPositionTable = nullptr;
    int*                                    indexTable = nullptr;
    int*                                    optimizeIndexTable = nullptr;
    bool                                    overwritePosOn = false;

    HorizontalContainer(std::string str, int z, Color col, Rectangle pos, ContainerType t, int columns, int lines, float left, float up, float right, float down, float space);

    void                                    InitSize();
    int                                     AssignPos();
    void                                    OverwritePos();

    ~HorizontalContainer();
};

CardContainer ExtractNCardsFrom(CardContainer& container, int n);
