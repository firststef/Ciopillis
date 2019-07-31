#include "nvidia.h"
#include <raylib.h>
#include "Helpers.h"
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
    manager.systemManager->AddSystem(dynamic_pointer_cast<ISystem>(std::make_shared<DrawSystem>(DrawSystem())));
    manager.systemManager->AddSystem(dynamic_pointer_cast<ISystem>(std::make_shared<MouseInputSystem>(MouseInputSystem())));
    manager.systemManager->AddSystem(dynamic_pointer_cast<ISystem>(std::make_shared<EventSystem>(EventSystem())));
    
    auto newPLayer(manager.pool->AddEntity());
    newPLayer->Add<TransformComponent>();
    newPLayer->Get<TransformComponent>().rectangle = {500,500, 300, 600 };
    newPLayer->Add<SpriteComponent>();
    newPLayer->Add<MouseInputComponent>();

    auto card(manager.pool->AddEntity());
    card->Add<TransformComponent>();
    card->Get<TransformComponent>().rectangle = { 500,500, 300, 600 };
    card->Add<SpriteComponent>().color = BLACK;
    card->Add<MouseInputComponent>();

    InitWindow(screenWidth, screenHeight, windowTitle);
    SetTargetFPS(60);

    manager.Initialize();
    while (!WindowShouldClose())
    {
        manager.Update();
    }

    return 0;
}