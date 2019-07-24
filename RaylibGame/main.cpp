#include "nvidia.h"
#include "Engine.h"

using namespace std;

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be
//easy to locate their place - perhaps some option added to the input manager

*/
//-----[Globals]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int const screenWidth = 1600;
int const screenHeight = 900;

const int FONT_SIZE = 50;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int enabledGestures = 0b0000000000001111;

int main()
{
    Manager manager;

    /*-----[RESOURCES]----------------------------------------------------------*/

    CardContainer* cardDatabase = new CardContainer;

    std::string name("The First Board");
    Board* board = new Board(
        name,
        -1,
        LIGHTGRAY,
        { 0, 0, screenWidth, screenHeight }
        );
    manager.activeObjects.PlaceChild(static_cast<GameObject>(*board), 0, LAST_IDX(manager.activeObjects));

    Card* card = new Card(std::string ("GREEN"),1, GREEN, { screenWidth / 2,screenHeight / 2, 225 , 375 });
    cardDatabase->cards.emplace_back(card);

    Card* card1 = new Card(std::string ("BLUE") , 2, BLUE, { screenWidth / 2,screenHeight / 2, 225 , 375 });
    cardDatabase->cards.emplace_back(card1);

    Card* card2 = new Card(std::string ("RED") , 2, RED, { screenWidth / 2,screenHeight / 2, 225 , 375 });
    cardDatabase->cards.emplace_back(card2);

    Card* card3 = new Card(std::string ("BLACK"), 4 ,BLACK ,{ screenWidth / 2,screenHeight / 2, 225 , 375 });
    cardDatabase->cards.emplace_back(card3);

    Card* card4 = new Card(std::string ("PINK"),4, PINK, { screenWidth / 2,screenHeight / 2, 225 , 375 });
    cardDatabase->cards.emplace_back(card4);

    /*-----[GAME]-----------------------------------------------------------------------------------------------------------------------------*/

    CardContainer draw = ExtractNCardsFrom(*cardDatabase, 3);

    /*AddObjectToArray<Owner, Container>(
        Manager::activeObjects.children,
        *(static_cast<Container*>(hand)),
        0,
        Manager::activeObjects.children.size() - 1,
        nullptr
    );//this should be a gamemanager object function*/

    HorizontalContainer* playerHand = new HorizontalContainer(
        std::string("PlayerHand"),
        4,
        PURPLE,
        {
            screenWidth / 4,
            screenHeight / 4,
            screenWidth / 2,
            screenHeight / 2
        },
        Container::MATERIAL,
        4,
        1,
        10,
        10,
        10,
        10,
        20
    );
    for (auto _card = draw.cards.begin(); _card != draw.cards.end(); ++_card)
    {
        playerHand->PlaceChild((static_cast<GameObject>(*_card)),0, LAST_IDX((*playerHand)));
    }
    playerHand->isActive = true;
    for (auto own = playerHand->children.rbegin(); own != playerHand->children.rend(); ++own)
    {
        static_cast<GameObject*>(own->GetPointer())->isActive = true;
        static_cast<GameObject*>(own->GetPointer())->isSelectable = true;
    }
    playerHand->type = Container::MATERIAL;
    playerHand->stretchEnabled = false;

    manager.activeObjects.PlaceChild(
        *(static_cast<Container*>(playerHand)),
        0,
        LAST_IDX(manager.activeObjects)
    );

    std::string log;    
    Entity player(std::string("Player") );
    Entity enemy(std::string("Enemy"));
    GameServer server(GameServer::Interface::CONSOLE, log, *cardDatabase,player,enemy);

    system("CLS");
    while (!WindowShouldClose())
    {
        //CHECKBK(server.RunConsole() != 0, "Server exited");
        
        //Ok deci logica de baza
        
        Input input = manager.inputManager.ListenToInput();
        //inregistreaza inputul superficial
        Action action = manager.actionManager.InterpretInput(input);//------------ poate o sa fie nevoie sa trecem pe multithreading aici - animatia sa continue cat timpin input scrie dragging de ex.
        //observa inputul primit si il compara cu state-ul curent
        //daca este valid, mai intai salveaza state-ul curent
        //-----(ceea ce inseamna ca o sa avem nevoie de niste functii
        //Copy pt Container si GameObject plus Remove() tot pentru 
        //astea 2)
        //dupa care schimba putin state-ul obiectelor (poate muta
        //obiectele care nu au atributul Locked sau NonMovable etc.)
        Action response = manager.gameManager.ValidateAction(action);
        //verifica cu state-ul jocului daca mutarea este permisa
        //si intoarce rezultatul validarii
        manager.actionManager.InterpretResponse(response);
        //functiile astea pot folosi niste API uri interne de exemplu
        //moveObject() sau cv
        //aici practic finalizeaza actiunea -- ar trebui facu ceva gen o coada
        //pentru a face animatiile cum trebuie, sau un fel de clasa animatie
        //care intoarce position si rotation in functie de timpul de start
        //si timpul actual
        manager.screenManager.Draw();
        //deocamdata aici doar deseneaza - la un moment dat poate
        //face in functie de response ceva - desi acum ca ma gandesc action
        //manager este acum cel care se ocupa cu tot - bine, ce-i drept
        //daca ne gandim la animatii, screenManager mai mult se ocupa de 
        //mecanici, fiindca ii trebuie pentru gameManager
    }

    //cardDatabase->Destroy();
    //eroarea de astazi - imi crapa pentru ca dadeam valorile pointerilor din cardDatabase
    //si erau copiati in active objects iar apoi active objects le distrugea la final dar 
    //ele erau deja distruse de cardDatabase
    //am rezolvat asta facand copii dinamice
    //o eroare pe care am avut-o a fost ca facea copii de GameObject* pentru ca nu am facut o
    //functie virtuala pt Card ca sa ii intoarca un obiect cu functia potrivita de Draw()
    //pentru carte => apela Draw() din GO si nu se vedea nimic

    return 0;
}