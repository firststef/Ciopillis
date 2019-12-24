#include <raylib.h>
#include <ECSlib.h>
#include "Constants.h"
#include "ArenaSystem.h"
#include "ArenaEventSystem.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerEvent.h"

int main()
{
    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    ECSManager manager;

    auto& defferSystem = manager.systemManager.AddSystem<DefferSystem>();
    auto& arenaSystem = manager.systemManager.AddSystem<ArenaSystem>();
    auto& arenaEventSystem = manager.systemManager.AddSystem<ArenaEventSystem>();
    auto& drawSystem = manager.systemManager.AddSystem<DrawSystem>();
    auto& keyBoardInputSytem = manager.systemManager.AddSystem<KeyboardInputSystem>();
    auto& physicsSystem = manager.systemManager.AddSystem<PhysicsSystem>();
    auto& animationSystem = manager.systemManager.AddSystem<AnimationSystem>();
    auto& hitBoxSystem = manager.systemManager.AddSystem<HitBoxSystem>();

    auto game(manager.pool.AddEntity());
    game->Add<ArenaGameComponent>();

    manager.eventManager.Subscribe<DefferEvent>(&defferSystem);
    manager.eventManager.Subscribe<KeyboardEvent>(&arenaEventSystem);
    manager.eventManager.Subscribe<ArenaPlayerEvent>(&arenaSystem);
    manager.eventManager.Subscribe<HitBoxEvent>(&arenaSystem);

    manager.Initialize();

    while (!WindowShouldClose())
    {
        manager.Update();
    }

    manager.Destroy();

    return 1;
}
