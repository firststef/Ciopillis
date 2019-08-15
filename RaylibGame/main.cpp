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

        //Player
    auto hand(manager.pool.AddEntity());
    auto draw(manager.pool.AddEntity());
    auto discard(manager.pool.AddEntity());
    auto playZone(manager.pool.AddEntity());
    auto endturnButton(manager.pool.AddEntity());
    
    auto playerDrawCard(manager.pool.AddEntity());

        //Enemy
    auto enemyHand(manager.pool.AddEntity());
    auto enemyDraw(manager.pool.AddEntity());
    auto enemyDiscard(manager.pool.AddEntity());

    auto enemyDrawCard(manager.pool.AddEntity());

    hand->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2, SCREEN_HEIGHT - HAND_BOARD_HEIGHT, HAND_BOARD_WIDTH, HAND_BOARD_HEIGHT });
    hand->Add<SpriteComponent>(std::string("Hand"), Color(PURPLE));
    hand->Add<GridContainerComponent>(5, 1, 10, 10, 10, 10, -200, false, GridContainerComponent::DYNAMIC_ERASE_SPACES, true);
    
    discard->Add<TransformComponent>(Rectangle{ 70, SCREEN_HEIGHT - 40 - CARD_HEIGHT, CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    discard->Add<SpriteComponent>(std::string("Discard"), Color(BLACK));
    discard->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::INFINITE_STACK);

    draw->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - 90 - CARD_WIDTH, SCREEN_HEIGHT - 40 - CARD_HEIGHT , CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    draw->Add<SpriteComponent>(std::string("Draw"), Color(RED));
    draw->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 20, false, GridContainerComponent::INFINITE_STACK);

    playZone->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2 + 80, 160, HAND_BOARD_WIDTH - 160, 400 });
    playZone->Add<SpriteComponent>(std::string("Play Zone"), Color(GRAY));
    playZone->Add<GridContainerComponent>(1, 2, 10, 10, 10, 10, 0, false, GridContainerComponent::DYNAMIC_ERASE_SPACES, true);

    endturnButton->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 + HAND_BOARD_WIDTH / 2  - 60, SCREEN_HEIGHT/2 - 10, 120, 120 });
    endturnButton->Add<SpriteComponent>(std::string("Endturn Button"), Color(GREEN));
    endturnButton->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));

    playerDrawCard->Add<TransformComponent>(Rectangle{ -500,-500, CARD_WIDTH, CARD_HEIGHT });
    playerDrawCard->Add<SpriteComponent>(std::string("Draw Card"), manager.textureManager.Load("../cards/backface.png"));
    playerDrawCard->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::DRAG) | (1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));

    enemyHand->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2, 0, HAND_BOARD_WIDTH, 120 });
    enemyHand->Add<SpriteComponent>(std::string("Enemy Hand"), Color(DARKPURPLE));
    enemyHand->Add<GridContainerComponent>(5, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::DYNAMIC_ERASE_SPACES);

    enemyDraw->Add<TransformComponent>(Rectangle{ 70, 20, CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    enemyDraw->Add<SpriteComponent>(std::string("Enemy Draw"), Color(MAROON));
    enemyDraw->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::INFINITE_STACK);

    enemyDiscard->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - 90 - CARD_WIDTH, 20, CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    enemyDiscard->Add<SpriteComponent>(std::string("Enemy Discard"), Color(DARKGRAY));
    enemyDiscard->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 20, false, GridContainerComponent::INFINITE_STACK);

    enemyDrawCard->Add<TransformComponent>(Rectangle{ -500,-500, CARD_WIDTH, CARD_HEIGHT });
    enemyDrawCard->Add<SpriteComponent>(std::string("Enemy Draw Card"), manager.textureManager.Load("../cards/backface.png"));

    //SystemManager
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
    auto mouseInputSystem = std::make_shared<MouseInputSystem>(MouseInputSystem());
    auto eventSystem = std::make_shared<EventSystem>(EventSystem(server));
    auto enemySystem = std::make_shared<EnemySystem>(EnemySystem(server, enemyHand, enemyDraw, enemyDiscard, playZone));
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
    gridContainerSystem->AddItem(draw, playerDrawCard);
    gridContainerSystem->AddItem(enemyDraw, enemyDrawCard);

    while (!WindowShouldClose())
    {
        manager.Update();
    }

    return 0;
}