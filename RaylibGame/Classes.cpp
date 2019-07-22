#include  "Classes.h"

using namespace std;
using namespace  Types;

GameObject::GameObject(GameObject &obj)
{
    *this = obj;
    this->name = obj.name;
}
GameObject* GameObject::GetCopy()
{
    GameObject* go = new GameObject;
    *go = *this;
    return go;
}

void Board::Draw()
{
    DrawRectangleRec(position, color);
};

void Card::Draw()
{
    DrawRectangleRec(position, color);
};
GameObject* Card::GetCopy()
{
    Card* go = new Card;
    *go = *this;
    return go;
}

Owner::Owner(GameObject* pointer)
{
    go_pointer = pointer;
    index = 0;
}
Owner::Owner(Container* pointer)
{
    c_pointer = pointer;
    index = 1;
}
Owner& Owner::operator=(GameObject* pointer)
{
    go_pointer = pointer;
    index = 0;
    return *this;
}
Owner& Owner::operator=(Container* pointer)
{
    c_pointer = pointer;
    index = 1;
    return *this;
}
bool Owner::operator==(Owner& other)
{
    return go_pointer == other.go_pointer;
}
bool Owner::operator==(void* ptr)
{
    return go_pointer == ptr;
}
bool Owner::operator!=(Owner& other)
{
    return !(*this == other);
}
bool Owner::operator!=(void* ptr)
{
    return !(go_pointer == ptr);
}
GameObject* Owner::operator->()
{
    return go_pointer;
}
Owner::~Owner() {

}

Container* Container::GetCopy()
{
    Container* cont = new Container;
    cont->type = type;
    cont->zIndex = zIndex;
    for (auto obj = children.begin(); obj != children.end(); ++obj)
    {
        Owner* newOwner = new Owner;
        newOwner->MakeCopy(*obj);
        cont->children.emplace_back(*newOwner);
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
    for (auto obj = children.begin(); obj != children.end(); ++obj)
    {
        (*obj).Destroy();
    }
    children.clear();
}
Container::~Container()
{
    //Destroy(); --destroy no longer implicitly called
}

//From Owner
void Owner::MakeCopy(Owner& owner)
{
    this->index = owner.index;
    this->go_pointer = owner.go_pointer == nullptr ? nullptr : owner.go_pointer->GetCopy();
}
void Owner::Destroy()
{
    if (!alreadyDestroyed) {
        if (index == 0)
            delete go_pointer;
        else
            delete c_pointer;

        alreadyDestroyed = true;
    }
}

void CardContainer::Draw() {

    //draws himself first,
    if (type == OVERLAY || type == MATERIAL)
        DrawRectangleRec(position, color);

    //then the children
    if (type == WRAPPER || type == MATERIAL)
        for (auto card = children.begin(); card != children.end(); ++card) {
            auto genericObj = (*card).go_pointer;
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
Container* CardContainer::GetCopy()
{
    CardContainer* cont = new CardContainer;
    cont->type = type;
    cont->zIndex = zIndex;
    for (auto obj = children.begin(); obj != children.end(); ++obj)
    {
        Owner* newOwner = new Owner;
        newOwner->MakeCopy(*obj);
        cont->children.emplace_back(*newOwner);
    }
    return cont;
}

HorizontalContainer::HorizontalContainer(Types::SString name, Rectangle pos, int columns, int lines) :
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
HorizontalContainer::HorizontalContainer(Types::SString name, Rectangle pos, int columns, int lines, float left, float up, float right, float down, float space) :
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
    for (auto obj = children.begin(); obj != children.end(); ++obj) {
        auto genericObj = (*obj).go_pointer;

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
        for (auto obj = children.begin(); obj != children.end(); ++obj) {
            auto genericObj = (*obj).go_pointer;
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
CardContainer ExtractNCardsFrom(CardContainer& container, int n)
{
    CardContainer cont;
    vector<int> selectedIndexes;

    if (n > container.children.size())
        return cont;

    for (int i = n; n >= 1;) {
        int randomN = GetRandomValue(0, container.children.size() - 1);

        if ([&]()->bool {for (auto idx : selectedIndexes) { if (idx == randomN) return false; } return true; }()) {
            selectedIndexes.push_back(randomN);
            n--;
        }
    }

    for (auto it : selectedIndexes) {
        cont.AddChild(dynamic_cast<Card*>(container[it].go_pointer));
    }

    return cont;
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
Owner& GetGameObjectUnderPoint(Vector2 point, Container& container, int& order) {//add bool LookForParent - returns only the object that you are on top and is also parent
    Owner returnOwner;
    returnOwner.go_pointer = nullptr;

    for (auto variant_child = container.children.rbegin(); variant_child != container.children.rend() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 0)//GameObject
        {
            const auto ptr = (*variant_child).go_pointer;
            if (ptr && CheckCollisionPointRec(point, ptr->position))
            {

                if (ptr->zIndex != -1)
                {
                    returnOwner = (*variant_child);
                    --order;
                    continue;
                }

                returnOwner.go_pointer = nullptr;
                returnOwner.index = 0;
                --order;

            }
        }
        else {//Container
            const auto ownr = GetGameObjectUnderPoint(point, *((*variant_child).c_pointer), order);
            if (ownr.go_pointer) {
                if (ownr.go_pointer->zIndex != -1)
                {
                    returnOwner = (*variant_child);
                    --order;
                    continue;
                }

                returnOwner.go_pointer = nullptr;
                returnOwner.index = 1;
                --order;
            }
        }
    }

    return returnOwner;
}
//This function will return Container objects - if you specify true for getLogical it will also consider abstract containers
//You could combine GetParentUnderPoint and GetGameObjectUnderPoint to deduce if the object you are on has this parent,
//perhaps by iterating through numbers for order or somehow recursively reaching the parent
Owner& GetParentUnderPoint(Vector2 point, Container& container, int& order, bool getLogical) {//add bool LookForParent - returns only the object that you are on top and is also parent

    Owner returnOwner;
    returnOwner.go_pointer = nullptr;

    for (auto variant_child = container.children.rbegin(); variant_child != container.children.rend() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 1) {//Container

            if (getLogical || CheckCollisionPointRec(point, (*variant_child).c_pointer->position))
            {
                returnOwner = (*variant_child);
                --order;
            }

            if (order < 0)
                break;

            const auto ownr = GetParentUnderPoint(point, *((*variant_child).c_pointer), order, getLogical);

            returnOwner = ownr;

            if (order < 0)
                break;

        }
    }

    return returnOwner;
}
Owner& GetObjectUnderPoint(Vector2 point, Container& container, int& order) {

    Owner returnOwner;
    returnOwner.go_pointer = nullptr;

    for (auto variant_child = container.children.rbegin(); variant_child != container.children.rend() && order >= 0; ++variant_child) {
        if ((*variant_child).index == 0)//GameObject
        {
            const auto ptr = (*variant_child).go_pointer;
            if (ptr && CheckCollisionPointRec(point, ptr->position))
            {

                if (ptr->zIndex != -1)
                {
                    returnOwner = (*variant_child);
                    --order;
                    continue;
                }

                returnOwner.go_pointer = nullptr;
                returnOwner.index = 0;
                --order;

            }
        }
        else {//Container
            const auto ptr1 = (*variant_child).c_pointer;
            if (ptr1 && CheckCollisionPointRec(point, ptr1->position))
            {
                returnOwner = (*variant_child);
                --order;
            }

            if (order < 0)
                break;

            const auto ownr = GetObjectUnderPoint(point, *((*variant_child).c_pointer), order);

            returnOwner = ownr;

        }
    }

    return returnOwner;
}
//this actually tries to get a selectable object but still returns an object
Owner& GetSelectableObjectUnderPoint(Vector2 point, Container& container, int& order) {
    Owner obj;
    obj.go_pointer = nullptr;

    --order;
    int returnOrder = -1;
    do
    {
        ++order;
        int save = order;
        obj = GetObjectUnderPoint(point, container, order);
        returnOrder = order;
        order = save;
    } while (obj.index == 1 && !obj.c_pointer->isSelectable);
    order = returnOrder;

    return obj;
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

    while (idx <= endPos && object.zIndex >= (*(iterator + idx))->zIndex) {
        ++idx;
    }

    objectArray.insert(iterator + idx, &object);

    return true;//nu merge ordonarea
}
template<>
bool AddObjectToArray<Owner, GameObject>(vector< Owner > &objectArray, GameObject &object, int beginPos, int endPos, void* parent) {
    auto iterator = (!objectArray.empty()) ? objectArray.begin() + endPos : objectArray.begin();
    GameObject* pointer = nullptr;

    for (auto obj = objectArray.rbegin(); obj != objectArray.rend() - beginPos; ++obj)
    {
        pointer = (*obj).go_pointer;

        if (*obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

    Owner* obj = new Owner(&object);
    obj->parent = static_cast<Container*>(parent);
    objectArray.insert(iterator, *obj);//aici apeleaza constructorul de copiere

    return true;
}
template<>
bool AddObjectToArray<Owner, Container>(vector< Owner > &objectArray, Container &object, int beginPos, int endPos, void* parent) {
    auto iterator = (!objectArray.empty()) ? objectArray.begin() + endPos : objectArray.begin();
    GameObject* pointer = nullptr;

    for (auto obj = objectArray.rbegin(); obj != objectArray.rend() - beginPos; ++obj)
    {
        pointer = (*obj).go_pointer;

        if (*obj == objectArray[objectArray.empty() ? 0 : objectArray.size() - 1])
        {
            if (object.zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (object.zIndex < pointer->zIndex && iterator != objectArray.begin())
            --iterator;
    }

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

    return AddObjectToArray(objectArray, object, 0, objectArray.size() - 1);
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
//Both beginPos and endPos represent indexes of the limit-elements with which the object will be compared
bool AddObjectToContainer(Container &container, Owner &owner, int beginPos, int endPos)
{
    auto iterator = (!container.children.empty()) ? container.children.begin() + endPos : container.children.begin();
    GameObject* pointer = nullptr;

    for (auto obj = container.children.rbegin(); obj != container.children.rend() - beginPos; ++obj)
    {
        pointer = (*obj).go_pointer;

        if (*obj == container.children[container.children.empty() ? 0 : container.children.size() - 1])
        {
            if (owner.go_pointer->zIndex >= pointer->zIndex)
                ++iterator;
        }
        else if (owner.go_pointer->zIndex < pointer->zIndex && iterator != container.children.begin())
            --iterator;
    }

    owner.parent = &container;
    container.children.insert(iterator, owner);

    return true;
}
bool ResetPositionInContainer(Container &container, Owner &owner, int beginPos, int endPos, bool(*func)(Container &cont, Owner &obj))
{
    auto begin = container.children.begin();

    int idx = beginPos;
    for (; idx != endPos + 1; ++idx) {
        if ((*(begin + idx)) == owner) {
            container.children.erase(begin + idx);
            break;
        }
    }

    func(container, owner);

    if (idx < beginPos)
    {
        --beginPos; --endPos;
    }
    else if (idx < endPos)
        --endPos;

    return AddObjectToContainer(container, owner, beginPos, endPos);
}
//trebuie neaparat o functie de defragmentare