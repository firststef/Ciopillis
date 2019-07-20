#define _CRT_NONSTDC_NO_DEPRECATE
#include "Classes.h"
#include "Engine.h"

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*


//"Card" MACRO_CONCAT("", __COUNTER__)

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be
//easy to locate their place - perhaps some option added to the input manager

Ok so GameManager will be the big class that manages:

    Gets data from input manager and
    Sends the data to event Manager
    The event Manager initializes a turn data
    Turn data is processed (probably by GameManager)
    At last screen manager is also called and processes what to do

   Note: Communication between the classes could be made by something
like a buffer (pipe) that communicates between the classes

   Note: The event that is sent between the input and the eventManager
might be changed frequently

*/

//-----[Globals]---------------------------------------------------------------------------------------------------------------------------------------------------------------

const int screenWidth = 1600;
const int screenHeight = 800;

const int FONT_SIZE = 50;

//Input Globals
constexpr float timeForDragDelay = 0.000000000001f;
static_assert(timeForDragDelay != 0, "timeForDragDelay must not be 0");
float temporayTimeForDragDelay = timeForDragDelay;
float dragDuration = 0;
Vector2 mouseGrab = { 0,0 };
bool DragStarted = false;
GameObject* dragSelectedObject = nullptr;//current selected object
float endPositionX = -1;
float endPositionY = -1;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int enabledGestures = 0b0000000000001001;

int main()
{
    ScreenManager screenManager(screenWidth, screenHeight);
    GameManager gameManager;
    InputManager inputManager(enabledGestures);
    ActionManager actionManager;

    /*-----[RESOURCES]----------------------------------------------------------*/

    vector<Card*> cardDatabase;

    SString name("The First Board");
    Board* board = new Board;
    board->name = name;
    board->position = { 0, 0, screenWidth, screenHeight };
    board->zIndex = -1;
    Manager::activeObjects.AddChild(board);//sar putea pur si simplu sa nu fie nevoie de acel destroy ci pur si simplu sa fac in interiorul functiilor un auxiliar dinamic

    SString numeCarte("Carte");
    Texture2D texture = { 0 };
    Card* card = new Card(numeCarte, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture);
    card->zIndex = 1;
    card->color = GREEN;

    SString numeCarte1("Carte1");
    Texture2D texture1 = { 0 };
    Card* card1 = new Card(numeCarte1, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture1);
    card1->zIndex = 2;
    card1->color = BLUE;

    SString numeCarte2("Carte2");
    Texture2D texture2 = { 0 };
    Card* card2 = new Card(numeCarte2, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture2);
    card2->zIndex = 2;
    card2->color = RED;

    SString numeCarte3("Carte3");
    Texture2D texture3 = { 0 };
    Card* card3 = new Card(numeCarte3, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture3);
    card3->zIndex = 4;
    card3->color = BLACK;

    SString numeCarte4("Carte4");
    Texture2D texture4 = { 0 };
    Card* card4 = new Card(numeCarte4, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture4);
    card4->zIndex = 4;
    card4->color = PINK;

    cardDatabase.emplace_back(card);
    cardDatabase.emplace_back(card1);
    cardDatabase.emplace_back(card2);
    cardDatabase.emplace_back(card3);
    cardDatabase.emplace_back(card4);

    /*-----[GAME]-----------------------------------------------------------------------------------------------------------------------------*/

    SString hand_name("Hand");
    CardContainer* hand = new CardContainer(hand_name, { 0,0,0,0 });
    auto draw = ExtractNCardsFrom(cardDatabase, 3);
    hand->AddList(draw);
    hand->type = Container::WRAPPER;

    for (auto _card : cardDatabase) {
        _card->isActive = false;
    }

    for (auto _card = hand->children.begin(); _card != hand->children.end(); ++_card) {
        GameObject* pointer = nullptr;
        if (!(*_card).index)
            pointer = (*_card).go_pointer;
        if (pointer)
        {
            pointer->isActive = true;
            pointer->isSelectable = true;
        }
    }

    /*AddObjectToArray<Owner, Container>(
        Manager::activeObjects.children,
        *(static_cast<Container*>(hand)),
        0,
        Manager::activeObjects.children.size() - 1,
        nullptr
    );//this should be a gamemanager object function*/

    HorizontalContainer* playerHand = new HorizontalContainer(
        SString("PlayerHand"),
        {
            screenWidth / 4,
            screenHeight / 4,
            screenWidth / 2,
            screenHeight / 2
        },
        4,
        1,
        10,
        10,
        10,
        10,
        20
    );
    for (auto _card = hand->children.begin(); _card != hand->children.end(); ++_card)
    {
        GameObject* ptr = (*_card).go_pointer;
        playerHand->AddChild(ptr);
    }
    hand->children.clear();
    hand->isActive = false;
    playerHand->isActive = true;
    playerHand->type = Container::MATERIAL;
    playerHand->stretchEnabled = false;

    AddObjectToArray<Owner, Container>(
        Manager::activeObjects.children,
        *(static_cast<Container*>(playerHand)),
        0,
        Manager::activeObjects.children.size() - 1,
        nullptr
    );

    while (!WindowShouldClose())
    {
        //system("CLS");

        //Ok deci logica de baza
        
        Input input = inputManager.ListenToInput();
        //inregistreaza inputul superficial
        Action action = actionManager.InterpretInput(input);
        //observa inputul primit si il compara cu state-ul curent
        //daca este valid, mai intai salveaza state-ul curent
        //-----(ceea ce inseamna ca o sa avem nevoie de niste functii
        //Copy pt Container si GameObject plus Remove() tot pentru 
        //astea 2)
        //dupa care schimba putin state-ul obiectelor (poate muta
        //obiectele care nu au atributul Locked sau NonMovable etc.)
        Action response = gameManager.ValidateAction(action);
        //verifica cu state-ul jocului daca mutarea este permisa
        //si intoarce rezultatul validarii
        actionManager.InterpretResponse(response);
        //functiile astea pot folosi niste API uri interne de exemplu
        //moveObject() sau cv
        //aici practic finalizeaza actiunea
        screenManager.Draw();
        //deocamdata aici doar deseneaza - la un moment dat poate
        //face in functie de response ceva - desi acum ca ma gandesc action
        //manager este acum cel care se ocupa cu tot - bine, ce-i drept
        //daca ne gandim la animatii, screenManager mai mult se ocupa de 
        //mecanici, fiindca ii trebuie pentru gameManager
    }

    return 0;
}