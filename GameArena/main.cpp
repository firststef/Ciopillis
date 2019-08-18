#include "raylib.h"
#include "ECSlib.h"
#include "Constants.h"
#include "ArenaEvent.h"
#include "KeyboardInputComponent.h"
#include "KeyboardEvent.h"
#include "KeyboardInputSystem.h"
#include "ArenaEventSystem.h"
#include "ArenaGameComponent.h"
#include "ArenaSystem.h"

int main()
{
    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    ECSManager manager;

    

    auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem());
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());

    manager.systemManager.AddSystem(drawSystem);
    manager.systemManager.AddSystem(arenaSystem);

    game->Add<ArenaGameComponent>();

    while (!WindowShouldClose())
    {
        manager.Update();
        int ok = 0;
        while(ok)
        {
            
            ok = 0;
        }
    }

    return 1;
}