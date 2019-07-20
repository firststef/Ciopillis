#include "Classes.h"
#include "Engine.h"

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be
//easy to locate their place - perhaps some option added to the input manager

*/

//-----[Globals]---------------------------------------------------------------------------------------------------------------------------------------------------------------

const int screenWidth = 1600;
const int screenHeight = 800;

const int FONT_SIZE = 50;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int enabledGestures = 0b0000000000001111;

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
    Manager::activeObjects.AddChild(board);

    SString numeCarte("GREEN");
    Texture2D texture = { 0 };
    Card* card = new Card(numeCarte, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture);
    card->zIndex = 1;
    card->color = GREEN;

    SString numeCarte1("BLUE");
    Texture2D texture1 = { 0 };
    Card* card1 = new Card(numeCarte1, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture1);
    card1->zIndex = 2;
    card1->color = BLUE;

    SString numeCarte2("RED");
    Texture2D texture2 = { 0 };
    Card* card2 = new Card(numeCarte2, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture2);
    card2->zIndex = 2;
    card2->color = RED;

    SString numeCarte3("BLACK");
    Texture2D texture3 = { 0 };
    Card* card3 = new Card(numeCarte3, { screenWidth / 2,screenHeight / 2, 225 , 375 }, texture3);
    card3->zIndex = 4;
    card3->color = BLACK;

    SString numeCarte4("PINK");
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
        Action action = actionManager.InterpretInput(input);//------------ poate o sa fie nevoie sa trecem pe multithreading aici - animatia sa continue cat timpin input scrie dragging de ex.
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
        //aici practic finalizeaza actiunea -- ar trebui facu ceva gen o coada
        //pentru a face animatiile cum trebuie, sau un fel de clasa animatie
        //care intoarce position si rotation in functie de timpul de start
        //si timpul actual
        screenManager.Draw();
        //deocamdata aici doar deseneaza - la un moment dat poate
        //face in functie de response ceva - desi acum ca ma gandesc action
        //manager este acum cel care se ocupa cu tot - bine, ce-i drept
        //daca ne gandim la animatii, screenManager mai mult se ocupa de 
        //mecanici, fiindca ii trebuie pentru gameManager
    }

    return 0;
}