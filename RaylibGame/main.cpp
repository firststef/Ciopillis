#define _CRT_NONSTDC_NO_DEPRECATE
#include "Classes.h"

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

    SString str("Active");
    Container activeObjects(str);

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
    AddObjectToArray<ContWrapper, Container>(activeObjects.children,*(static_cast<Container*>(&hand)), 0, activeObjects.children.size() - 1, nullptr);//this should be a gamemanager object function

    while (!WindowShouldClose())
    {
        int lastGesture = GetGestureDetected();

        Vector2 mouse = GetMousePosition();
        float delta = GetFrameTime();

        system("CLS");

        //if lastGesture != gesture none inseamna ca userul face ceva, deci trebuie salvata inainte locatia cartilor si apoi dupa ce se reincepe logica,
        //trebuie validata pozitia si restabilita memoria

        int order = 0;
        auto ptr1 = GetParentUnderPoint(mouse, activeObjects, order, true);
        if (ptr1)
            cout <<"Parent:"<< ptr1->name<<endl;

        order = 0;
        auto ptr2 = GetObjectUnderPoint(mouse, activeObjects, order);
        if (ptr2)
            cout << "Object:" << ptr2->name << endl;

        order = 0;
        auto ptr3 = GetGameObjectUnderPoint(mouse, activeObjects, order);
        if (ptr3)
            cout << "GameObject:" << ptr3->name << endl;

        iff(!DragStarted && lastGesture == GestureType::GESTURE_DRAG) {

            //if drag just started -> init
            int order = 0;
            dragSelectedObject = GetGameObjectUnderPoint(mouse, activeObjects, order);

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
                },
                    &hand);
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
            int order = 0;
            if (dragSelectedObject != GetGameObjectUnderPoint(mouse, activeObjects, order))
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

