#include "nvidia.h"
#include <raylib.h>
#include "ECS.h"
#include "Components.h"
#include "Systems.h"

using namespace std;

//-----[Notes]---------------------------------------------------------------------------------------------------------------------------------------------------------------
/*

//i could do a free roam option - you can drag around the cards and at some point when you want to tidy up,
//you can click a button to bring them back - being that they are all stored in the memory, it  should be
//easy to locate their place - perhaps some option added to the input manager

*/
//-----[Globals]---------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* windowTitle = "raylib [core] example - mouse input";

int const screenWidth = 1600;
int const screenHeight = 900;

const int FONT_SIZE = 50;

//-----[Main]---------------------------------------------------------------------------------------------------------------------------------------------------------------

int enabledGestures = 0b0000000000001111;

int main()
{
    ECSManager manager;

    //SystemManager
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
    auto mouseInputSystem = std::make_shared<MouseInputSystem>(MouseInputSystem());
    auto eventSystem = std::make_shared<EventSystem>(EventSystem());
    auto gridContainerSystem = std::make_shared<GridContainerSystem>(GridContainerSystem());

    manager.systemManager.AddSystem(drawSystem);
    manager.systemManager.AddSystem(mouseInputSystem);
    manager.systemManager.AddSystem(eventSystem);
    manager.systemManager.AddSystem(gridContainerSystem);

    //EventManager
    manager.eventManager.Subscribe<MouseEvent>(eventSystem);

    //Pool
    auto board(manager.pool.AddEntity());
    auto card(manager.pool.AddEntity());
    auto card1(manager.pool.AddEntity());
    auto card2(manager.pool.AddEntity());

    card->Add<TransformComponent>();
    card->Get<TransformComponent>().position = { 500,500, 300, 600 };
    card->Add<SpriteComponent>().color = BLACK;
    card->Add<MouseInputComponent>();
    
    card1->Add<TransformComponent>();
    card1->Get<TransformComponent>().position = { 500,500, 300, 600 };
    card1->Add<SpriteComponent>().color = GREEN;
    card1->Add<MouseInputComponent>();

    card2->Add<TransformComponent>();
    card2->Get<TransformComponent>().position = { 500,500, 300, 600 };
    card2->Add<SpriteComponent>().color = BLUE;
    card2->Add<MouseInputComponent>();

    board->Add<TransformComponent>();
    board->Get<TransformComponent>().position = {screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 2};
    board->Add<SpriteComponent>();
    board->Add<GridContainerComponent>(4, 1, 10, 10, 10, 10, 20);

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    manager.Initialize();

    gridContainerSystem->AddItem(board, card);
    gridContainerSystem->AddItem(board, card1);
    gridContainerSystem->AddItem(board, card2);

    while (!WindowShouldClose())
    {
        manager.Update();
        int ok = 0;
        if (ok)//turn ok on while running in debug
        {
            gridContainerSystem->ReleaseItem(board, card2);
        }
    }

    return 0;
}