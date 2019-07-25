#include  "Classes.h"

GameObject::GameObject(std::string str, int z, Color col, Rectangle pos)
    : name(std::move(str)), zIndex(z), color(col), position(pos), draw([&]()->void {})
{

}

Board::Board(std::string str, int z, Color col, Rectangle pos) : GameObject(std::move(str), z, col, pos)
{
    draw = [&]()->void {DrawRectangleRec(position, color); };
}

Card::Card(std::string str, int z, Color col, Rectangle pos) : GameObject(std::move(str), z, col, pos)
{
    draw = [&]()->void {DrawRectangleRec(position, color); };
}

Owner::Owner(const Owner& other)
{
    memcpy(this, &other, sizeof(Owner));
    (const_cast<Owner*>(&other))->parent = nullptr;
    if (other.pointer.index() == 0)
        std::get<std::unique_ptr<GameObject>>((const_cast<Owner*>(&other))->pointer).reset(nullptr);

    //aici e clar periculos
}
void Owner::operator=(const Owner& other)
{
    memcpy(this, &other, sizeof(Owner));
}

void Owner::SetGameObject(GameObject go)
{
    pointer = std::make_unique<GameObject>(std::move(go));
}
void Owner::SetContainer(Container c)
{
    pointer = std::make_unique<Container>(std::move(c));
}
int Owner::GetZIndex()
{
    if (pointer.index() == 0)
    {
        return std::get<std::unique_ptr<GameObject>>(pointer)->zIndex;
    }
    else
    {
        return std::get<std::unique_ptr<Container>>(pointer)->zIndex;
    }
}
Rectangle Owner::GetPosition()
{
    if (pointer.index() == 0)
    {
        return std::get<std::unique_ptr<GameObject>>(pointer)->position;
    }
    else
    {
        return std::get<std::unique_ptr<Container>>(pointer)->position;
    }
}

bool Owner::GetIsActive()
{
    if (pointer.index() == 0)
    {
        return std::get<std::unique_ptr<GameObject>>(pointer)->isActive;
    }
    else
    {
        return std::get<std::unique_ptr<Container>>(pointer)->isActive;
    }
}
bool Owner::GetIsSelectable()
{
    if (pointer.index() == 0)
    {
        return std::get<std::unique_ptr<GameObject>>(pointer)->isActive;
    }
    else
    {
        return std::get<std::unique_ptr<Container>>(pointer)->isActive;
    }
}
std::function<void()> Owner::GetDraw()
{
    if (pointer.index() == 0)
    {
        return std::get<std::unique_ptr<GameObject>>(pointer)->draw;
    }
    else
    {
        return std::get<std::unique_ptr<Container>>(pointer)->draw;
    }
}
void* Owner::GetPointer()
{
    if (pointer.index() == 0)
    {
        return static_cast<void*>(std::get<std::unique_ptr<GameObject>>(pointer)._Myptr());
    }
    else
    {
        return  static_cast<void*>(std::get<std::unique_ptr<Container>>(pointer)._Myptr());
    }
}

Container::Container(std::string str, int z, Color col, Rectangle pos, ContainerType t) 
: GameObject(std::move(str), z, col, pos), type(t)
{
    draw = [&]()->void
    {
        //Draws himself
        DrawRectangleRec(position, color);

        //Then his children
        for (auto obj = children.begin(); obj != children.end(); ++obj)
        {
            obj->GetDraw()();
        }
    };
}
void Container::PlaceChild(Container obj, int beginPos, int endPos) {
    auto iterator = (!children.empty()) ? children.begin() + endPos : children.begin();

    for (auto own = children.rbegin(); own != children.rend() - beginPos; ++own)
    {   
        if (*own == children[children.empty() ? 0 : children.size() - 1])
        {
            if (obj.zIndex >= own->GetZIndex())
                ++iterator;
        }
        else if (obj.zIndex < own->GetZIndex() && iterator != children.begin())
            --iterator;
    }

    Owner newOwn;
    newOwn.SetContainer(obj);
    newOwn.parent = this;
    children.insert(iterator, newOwn);

    this->zIndex = MAX(this->zIndex, children.back().GetZIndex());
}
void Container::PlaceChild(GameObject obj, int beginPos, int endPos) {
    auto iterator = (!children.empty()) ? children.begin() + endPos : children.begin();

    for (auto own = children.rbegin(); own != children.rend() - beginPos; ++own)
    {
        if (*own == children[children.empty() ? 0 : children.size() - 1])
        {
            if (obj.zIndex >= own->GetZIndex())
                ++iterator;
        }
        else if (obj.zIndex < own->GetZIndex() && iterator != children.begin())
            --iterator;
    }

    Owner newOwn;
    newOwn.SetGameObject(obj);
    newOwn.parent = this;
    children.insert(iterator, newOwn);

    this->zIndex = MAX(this->zIndex, children.back().GetZIndex());
}
void Container::Destroy()
{
    children.clear();
}
void Container::ResetChildPosition(Owner* child, int beginPos, int endPos, bool(* func)(Container& cont, Owner* obj))
{
    auto begin = children.begin();

    int idx = beginPos;
    for (; idx != endPos + 1; ++idx) {
        if ((*(begin + idx)) == *child) {
            children.erase(begin + idx);
            break;
        }
    }

    func(*this, child);

    if (idx < beginPos)
    {
        --beginPos; --endPos;
    }
    else if (idx < endPos)
        --endPos;

    if (child->pointer.index() == 0)
        return PlaceChild(*static_cast<GameObject*>(child->GetPointer()), beginPos, endPos);
    else
        return PlaceChild(*static_cast<Container*>(child->GetPointer()), beginPos, endPos);
}
Owner Container::RemoveChild(int idx)
{
    Owner ownr = std::move(children[idx]);
    auto it = children.begin();
    std::advance(it, idx);
    children.erase(it);
    return std::move(ownr);
}

//trebuie verificata daca se obtine & bine
Owner* Container::GetSelectableObjectUnderPoint(Vector2 point, int order)
{
    Owner* obj = nullptr;

    --order;
    int returnOrder = -1;
    do
    {
        ++order;
        int save = order;
        obj = GetObjectUnderPoint(point, order);
        returnOrder = order;
        order = save;
    } while (obj->GetZIndex() == 1 && !obj->GetIsSelectable());
    order = returnOrder;

    return obj;
}
Owner* Container::GetGameObjectUnderPoint(Vector2 point, int order)
{
    Owner* returnOwner = nullptr;

    for (auto variant_child = children.rbegin(); variant_child != children.rend() && order >= 0; ++variant_child) {
        if (variant_child->pointer.index() == 0)//GameObject
        {
            const auto ptr = &(*variant_child);
            if (ptr && CheckCollisionPointRec(point, ptr->GetPosition()))
            {

                if (ptr->GetZIndex() != -1)
                {
                    returnOwner = &(*variant_child);
                    --order;
                    continue;
                }

                returnOwner = nullptr;
                --order;

            }
        }
        else {//Container
            const auto ownr = std::get<std::unique_ptr<Container>>(variant_child->pointer)->GetGameObjectUnderPoint(point, order);
            if (ownr->GetPointer()) {
                if (ownr->GetZIndex() != -1)
                {
                    returnOwner = &(*variant_child);
                    --order;
                    continue;
                }

                returnOwner = nullptr;
                --order;
            }
        }
    }

    return returnOwner;
}
Owner* Container::GetObjectUnderPoint(Vector2 point, int order)
{
    Owner* returnOwner = nullptr;

    for (auto variant_child = children.rbegin(); variant_child != children.rend() && order >= 0; ++variant_child) {
        if (variant_child->pointer.index() == 0)//GameObject
        {
            const auto ptr = &(*variant_child);
            if (ptr && CheckCollisionPointRec(point, ptr->GetPosition()))
            {

                if (ptr->GetZIndex() != -1)
                {
                    returnOwner = &(*variant_child);
                    --order;
                    continue;
                }

                returnOwner = nullptr;
                --order;

            }
        }
        else {//Container
            const auto ptr1 = std::get<std::unique_ptr<Container>>(variant_child->pointer)._Myptr();
            if (ptr1 && CheckCollisionPointRec(point, ptr1->position))
            {
                returnOwner = &(*variant_child);
                --order;
            }

            if (order < 0)
                break;

            const auto ownr = std::get<std::unique_ptr<Container>>(variant_child->pointer)->GetObjectUnderPoint(point, order);

            returnOwner = ownr;
        }
    }

    return returnOwner;
}
Owner* Container::GetParentUnderPoint(Vector2 point, int order, bool getLogical)
{
    Owner* returnOwner = nullptr;

    for (auto variant_child = children.rbegin(); variant_child != children.rend() && order >= 0; ++variant_child) {
        if ((*variant_child).pointer.index() == 1) {//Container

            if (getLogical || CheckCollisionPointRec(point, (*variant_child).GetPosition()))
            {
                returnOwner = &(*variant_child);
                --order;
            }

            if (order < 0)
                break;

            const auto ownr = std::get<std::unique_ptr<Container>>(variant_child->pointer)->GetParentUnderPoint(point, order, getLogical);

            returnOwner = ownr;

            if (order < 0)
                break;

        }
    }

    return returnOwner;
}
Owner* Container::operator[] (int n)
{
    return &(children[n]);
}
bool Owner::operator==(Owner& other)
{
    return GetPointer() == other.GetPointer();
}
bool Owner::operator==(void* ptr)
{
    return GetPointer() == ptr;
}
bool Owner::operator!=(Owner& other)
{
    return !(*this == other);
}
bool Owner::operator!=(void* ptr)
{
    return !(GetPointer() == ptr);
}

HorizontalContainer::HorizontalContainer(std::string str, int z, Color col, Rectangle pos, ContainerType t, int columns, int lines,
    float left, float up, float right, float down, float space) : Container(std::move(str), z, col, pos, t)
{
    draw = [&]()->void
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
                obj->GetDraw()();
            }
    };
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
        auto genericObj = static_cast<GameObject*>((*obj).GetPointer());

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
        //genericObj->draw();
        index++;
    }
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
    std::vector<int> selectedIndexes;

    if (n > container.cards.size())
        return cont;

    for (int i = n; n >= 1;) {
        int randomN = GetRandomValue(0, container.cards.size() - 1);

        if ([&]()->bool {for (auto idx : selectedIndexes) { if (idx == randomN) return false; } return true; }()) {
            selectedIndexes.push_back(randomN);
            n--;
        }
    }

    for (auto it : selectedIndexes) {
        cont.cards.push_back(container.cards[it]);
    }

    return cont;
}
/*GameObject* GetGameObjectUnderPoint(Vector2 point, std::vector<GameObject*> &objectArray, int& order) {
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
//trebuie neaparat o functie de defragmentare*/