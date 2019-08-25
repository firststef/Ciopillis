#include "ECSlib.h"
#include "Constants.h"
#include "GameServer.h"
#include "CardGenerator.h"
#include "CardGameSystems.h"

using namespace std;

int enabledGestures = 0b0000000000001111;

int main()
{
    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    std::string gameLog;

    CardGenerator generator("../GameServer/card_database.json");

    Player player("Player", generator.container);
    Player computer("Computer", generator.container);

    GameServer server(GameServer::Interface::SERVER, gameLog, generator.container, player, computer);

    ECSManager manager;

    //Pool
    auto playZone(manager.pool.AddEntity());
    auto endturnButton(manager.pool.AddEntity());

    //Player
    auto hand(manager.pool.AddEntity());
    auto draw(manager.pool.AddEntity());
    auto discard(manager.pool.AddEntity());
    
    auto drawCard(manager.pool.AddEntity());
    auto drawCard2(manager.pool.AddEntity());

    //Enemy
    auto enemyHand(manager.pool.AddEntity());

    hand->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2, SCREEN_HEIGHT - HAND_BOARD_HEIGHT, HAND_BOARD_WIDTH, HAND_BOARD_HEIGHT }, STATIC_CONTAINERS_Z);
    hand->Add<SpriteComponent>(std::string("Hand"), Texture2D(), Color(PURPLE));
    hand->Add<GridContainerComponent>(5, 1, 10, 10, 10, 10, -50, false, GridContainerComponent::DYNAMIC_ERASE_SPACES, false);
    
    discard->Add<TransformComponent>(Rectangle{ 70, SCREEN_HEIGHT/2 - CARD_HEIGHT/2, CARD_WIDTH + 20, CARD_HEIGHT + 20 }, STATIC_CONTAINERS_Z);
    discard->Add<SpriteComponent>(std::string("Discard"), Texture2D(), Color(BLACK));
    discard->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::INFINITE_STACK,false);

    draw->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - 90 - CARD_WIDTH, SCREEN_HEIGHT / 2 - CARD_HEIGHT / 2, CARD_WIDTH + 20, CARD_HEIGHT + 20 }, STATIC_CONTAINERS_Z);
    draw->Add<SpriteComponent>(std::string("Draw"), Texture2D(), Color(RED));
    draw->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 20, false, GridContainerComponent::INFINITE_STACK,false);

    playZone->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2 + 80, SCREEN_HEIGHT/2 - 200, HAND_BOARD_WIDTH - 160, 400 }, STATIC_CONTAINERS_Z);
    playZone->Add<SpriteComponent>(std::string("Play Zone"), Texture2D(), Color(GRAY));
    playZone->Add<GridContainerComponent>(1, 2, 10, 10, 10, 10, 0, false, GridContainerComponent::DYNAMIC_ERASE_SPACES, true);

    endturnButton->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 + HAND_BOARD_WIDTH / 2  - 60, SCREEN_HEIGHT/2 - 10, 120, 120 }, STATIC_FIELD_Z);
    endturnButton->Add<SpriteComponent>(std::string("Endturn Button"), Texture2D(), Color(GREEN));
    endturnButton->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));

    drawCard->Add<TransformComponent>(Rectangle{ -500,-500, CARD_WIDTH, CARD_HEIGHT }, DRAW_CARD_Z);
    drawCard->Add<SpriteComponent>(std::string("Draw Card"), manager.textureManager.Load("../cards/backface.png"), Color(WHITE));
    drawCard->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::DRAG) | (1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));

    drawCard2->Add<TransformComponent>(Rectangle{ -500,-500, CARD_WIDTH, CARD_HEIGHT }, DRAW_CARD_Z);
    drawCard2->Add<SpriteComponent>(std::string("Draw Card 2"), manager.textureManager.Load("../cards/backface.png"), Color(WHITE));
    drawCard2->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::DRAG) | (1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));

    enemyHand->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2, 0, HAND_BOARD_WIDTH, 120 }, STATIC_CONTAINERS_Z);
    enemyHand->Add<SpriteComponent>(std::string("Enemy Hand"), Texture2D(), Color(DARKPURPLE));
    enemyHand->Add<GridContainerComponent>(5, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::DYNAMIC_ERASE_SPACES,true);

    //SystemManager
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
    auto mouseInputSystem = std::make_shared<MouseInputSystem>(MouseInputSystem());
    auto eventSystem = std::make_shared<EventSystem>(EventSystem(server));
    auto enemySystem = std::make_shared<EnemySystem>(EnemySystem(server, enemyHand, draw, discard, playZone));
    auto gridContainerSystem = std::make_shared<GridContainerSystem>(GridContainerSystem());

    manager.systemManager.AddSystem(drawSystem);
    manager.systemManager.AddSystem(mouseInputSystem);
    manager.systemManager.AddSystem(eventSystem);
    manager.systemManager.AddSystem(enemySystem);
    manager.systemManager.AddSystem(gridContainerSystem);

    //EventManager
    manager.eventManager.Subscribe<MouseEvent>(eventSystem);
    manager.eventManager.Subscribe<SystemControlEvent>(mouseInputSystem);
    manager.eventManager.Subscribe<GridAddRemoveEvent>(gridContainerSystem);
    manager.eventManager.Subscribe<EnemyEvent>(enemySystem);

    manager.Initialize();

    //Game actions
    gridContainerSystem->AddItem(draw, drawCard);
    gridContainerSystem->AddItem(draw, drawCard2);

    while (!WindowShouldClose())
    {
        manager.Update();//TODO: deck sharing fails
    }

    return 0;
}