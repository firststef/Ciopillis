// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "nvidia.h"
#include <raylib.h>
#include "Constants.h"
#include "ECS.h"
#include "Components.h"
#include "Systems.h"

using namespace std;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int enabledGestures = 0b0000000000001111;

int main()
{
    std::string gameLog;

    CardGenerator generator("../GameServer/card_database.json");

    Player player("Player", generator.container);
    Player computer("Computer", generator.container);

    GameServer server(GameServer::Interface::CONSOLE, gameLog, generator.container, player, computer);

    ECSManager manager;

    //SystemManager
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
    auto mouseInputSystem = std::make_shared<MouseInputSystem>(MouseInputSystem());
    auto eventSystem = std::make_shared<EventSystem>(EventSystem(server));
    auto gridContainerSystem = std::make_shared<GridContainerSystem>(GridContainerSystem());

    manager.systemManager.AddSystem(drawSystem);
    manager.systemManager.AddSystem(mouseInputSystem);
    manager.systemManager.AddSystem(eventSystem);
    manager.systemManager.AddSystem(gridContainerSystem);

    //EventManager
    manager.eventManager.Subscribe<MouseEvent>(eventSystem);
    manager.eventManager.Subscribe<GridAddRemoveEvent>(gridContainerSystem);

    //Pool
    auto hand(manager.pool.AddEntity());
    auto draw(manager.pool.AddEntity());
    auto discard(manager.pool.AddEntity());
    auto playZone(manager.pool.AddEntity());
    auto endturnButton(manager.pool.AddEntity());
    
    auto card(manager.pool.AddEntity());
    //auto card1(manager.pool.AddEntity());
    //auto card2(manager.pool.AddEntity());

    card->Add<TransformComponent>(Rectangle{ 500,500, CARD_WIDTH, CARD_HEIGHT });
    card->Add<SpriteComponent>(std::string("Card1"), Color(PINK));
    card->Add<MouseInputComponent>();
    
    /*card1->Add<TransformComponent>();
    card1->Get<TransformComponent>().position = { 500,500, CARD_WIDTH, CARD_HEIGHT };
    card1->Add<SpriteComponent>(std::string("Card2"), Color(GREEN));
    card1->Add<MouseInputComponent>();

    card2->Add<TransformComponent>();
    card2->Get<TransformComponent>().position = { 500,500, CARD_WIDTH, CARD_HEIGHT };
    card2->Add<SpriteComponent>(std::string("Card3"), Color(BLUE));
    card2->Add<MouseInputComponent>();*/

    hand->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2, SCREEN_HEIGHT - HAND_BOARD_HEIGHT, HAND_BOARD_WIDTH, HAND_BOARD_HEIGHT });
    hand->Add<SpriteComponent>(std::string("Hand"), Color(PURPLE));
    hand->Add<GridContainerComponent>(4, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::DYNAMIC_ERASE_SPACES);
    
    discard->Add<TransformComponent>(Rectangle{ 70, SCREEN_HEIGHT - HAND_BOARD_HEIGHT + 80 - CARD_HEIGHT - 10, CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    discard->Add<SpriteComponent>(std::string("Discard"), Color(BLACK));
    discard->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 0, false, GridContainerComponent::INFINITE_STACK);

    draw->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - 90 - CARD_WIDTH, SCREEN_HEIGHT - HAND_BOARD_HEIGHT + 80 - CARD_HEIGHT - 10, CARD_WIDTH + 20, CARD_HEIGHT + 20 });
    draw->Add<SpriteComponent>(std::string("Draw"), Color(RED));
    draw->Add<GridContainerComponent>(1, 1, 10, 10, 10, 10, 20, false, GridContainerComponent::INFINITE_STACK);

    endturnButton->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - 80 - CARD_WIDTH, SCREEN_HEIGHT - HAND_BOARD_HEIGHT + 130, CARD_WIDTH, 60 });
    endturnButton->Add<SpriteComponent>(std::string("Endturn Button"), Color(GREEN));
    endturnButton->Add<MouseInputComponent>();

    playZone->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH / 2 - HAND_BOARD_WIDTH / 2 + 80, 160, HAND_BOARD_WIDTH - 160, 400 });
    playZone->Add<SpriteComponent>(std::string("Play Zone"), Color(GRAY));
    playZone->Add<GridContainerComponent>(1, 2, 10, 10, 10, 10, 0, false, GridContainerComponent::FIXED_GET_FIRST_AVAILABLE);

    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    manager.Initialize();

    //Game actions
    gridContainerSystem->AddItem(draw, card);

    while (!WindowShouldClose())
    {
        manager.Update();
    }

    return 0;
}