#pragma once
#include "Types.h"
#include "raylib.h"
#include <iostream>
#include <vector>

using namespace std;

//-----[Macros]---------------------------------------------------------------------------------------------------------------------------------------------------------------

constexpr int MAX(int x, int y) { return ((x > y) ? x : y); }

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

//-----[Classes]---------------------------------------------------------------------------------------------------------------------------------------------------------------

class GameObject;
class Card;
class Board;
class Container;
struct Owner;
class CardContainer;

CardContainer ExtractNCardsFrom(CardContainer& container, int n);
bool                            AddObjectToArray(vector<GameObject*> &objectArray, GameObject &object);
template<typename T, typename K>
bool                            AddObjectToArray(vector<T> &objectArray, K &object, int beginPos, int endPos, void* parent);
template<>
bool                            AddObjectToArray<Owner, GameObject>(vector< Owner > &objectArray, GameObject &object, int beginPos, int endPos, void* parent);
template<>
bool                            AddObjectToArray<Owner, Container>(vector< Owner > &objectArray, Container &object, int beginPos, int endPos, void* parent);
Owner& GetSelectableObjectUnderPoint(Vector2 point, Container& container, int& order);
bool ResetPositionInContainer(Container &container, Owner &owner, int beginPos, int endPos, bool(*func)(Container &cont, Owner &obj));

class GameObject {
public:
    Types::SString                     name;
    int                         zIndex = -1;

    Color                       color = LIGHTGRAY;
    Rectangle                   position = { 0,0,0,0 };
    Texture2D                   texture = { 0 };

    bool                        isActive = true;
    bool                        isSelectable = false;

    GameObject()                = default;

    explicit GameObject(Types::SString name, Rectangle pos) : name(name), position(pos) {};

    GameObject(GameObject &obj);

    GameObject&                 operator=(const GameObject &obj) = default;

    virtual GameObject*         GetCopy();

    virtual void Draw()         {};

    virtual ~GameObject()       = default;
};

class Board : public GameObject {
public:

    Board()                     {}

    Board(Types::SString name, Rectangle pos) : GameObject(name, pos) {}

    void Draw()                 override;
};

class Card : public GameObject {
public:

    Card()                      {}

    Card(Types::SString name, Rectangle rectangle, Texture2D image) : GameObject(name, rectangle) {
        texture = image;
    }

    //ABILITY
    ///maybe add a function pointer for each
    ///or an event type

    void Draw()                 override;
    GameObject*                 GetCopy() override;
};

struct Owner
{
    union
    {
        GameObject*             go_pointer;
        Container*              c_pointer;
    };
    Container*                  parent = nullptr;
    char                        index = -1;//trebuie modificat in type

    bool                        alreadyDestroyed = false;

    Owner()                     = default;
    Owner(GameObject* pointer);
    Owner(Container* pointer);
    Owner(const Owner& other)   = default; //these two will not create new objects
    Owner(Owner&& other)        = default;

    Owner&                      operator=(const Owner& other) = default; //these two will not create new objects
    Owner&                      operator=(GameObject* pointer);
    Owner&                      operator=(Container* pointer);
    bool                        operator==(Owner& other);
    bool                        operator==(void* ptr);
    bool                        operator!=(Owner& other);
    bool                        operator!=(void* ptr);
    GameObject*                 operator->();
    void                        MakeCopy(Owner& owner);
    void                        Destroy();

    ~Owner();
};

class Container : public GameObject {
public:
    vector< Owner >             children;

    enum ContainerType {
        LOGICAL,// don't show object or his children
        WRAPPER,// shows only his children 
        OVERLAY,// show only object 
        MATERIAL// show object and his children
    }                           type = LOGICAL;

    Container()                 = default;
    Container(Container &cont)  { *this = cont; };
    explicit Container(Types::SString name, Rectangle pos) : GameObject(name,pos) {};

    virtual Container*          GetCopy();
    Owner&                      operator[](int n);
    void                        Draw();
    virtual void                AddChild(Container* obj);
    virtual void                AddChild(GameObject* obj);
    virtual void                Destroy();

    ~Container();

    friend Owner&                GetGameObjectUnderPoint(Vector2 point, Container& container, int& order);
};

class CardContainer : public Container {
public:

    CardContainer()             = default;
    CardContainer(CardContainer& cont) { *this = cont; }
    explicit CardContainer(Types::SString name, Rectangle pos) : Container(name, pos) {};

    void                        Draw();
    void                        AddList(vector<Card*> const& cards);
    virtual void                AddChild(Card* obj);
    Container*                  GetCopy() override;
};

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

    explicit HorizontalContainer(Types::SString name, Rectangle pos, int columns, int lines);
    explicit HorizontalContainer(int columns, int lines);
    explicit HorizontalContainer(Types::SString name, Rectangle pos, int columns, int lines, float left, float up, float right, float down, float space);
    explicit HorizontalContainer(int columns, int lines, float left, float up, float right, float down, float space);

    void InitSize();
    int AssignPos();
    void OverwritePos();
    void Draw();
    void AddChild(Container* obj);
    void AddChild(GameObject* obj);
    ~HorizontalContainer();
};
