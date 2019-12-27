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

	auto defferSystem = std::make_shared<DefferSystem>(DefferSystem());
	auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem());
	auto arenaEventSystem = std::make_shared<ArenaEventSystem>(ArenaEventSystem());
	auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
	auto keyboardInputSystem = std::make_shared<KeyboardInputSystem>(KeyboardInputSystem());
	auto physicsSystem = std::make_shared <PhysicsSystem>(PhysicsSystem());
	auto animationSystem = std::make_shared <AnimationSystem>(AnimationSystem());
	auto hitBoxSystem = std::make_shared<HitBoxSystem>(HitBoxSystem());
	auto networkSystem = std::make_shared<NetworkSystem>(NetworkSystem());

	manager.systemManager.AddSystem(defferSystem);
	manager.systemManager.AddSystem(drawSystem);
	manager.systemManager.AddSystem(arenaSystem);
	manager.systemManager.AddSystem(arenaEventSystem);
	manager.systemManager.AddSystem(keyboardInputSystem);
	manager.systemManager.AddSystem(physicsSystem);
	manager.systemManager.AddSystem(animationSystem);
	manager.systemManager.AddSystem(hitBoxSystem);
	manager.systemManager.AddSystem(networkSystem);

	auto game(manager.pool.AddEntity());
	game->Add<ArenaGameComponent>();

	manager.eventManager.Subscribe<DefferEvent>(defferSystem);
	manager.eventManager.Subscribe<KeyboardEvent>(arenaEventSystem);
	manager.eventManager.Subscribe<ArenaPlayerEvent>(arenaSystem);
	manager.eventManager.Subscribe<HitBoxEvent>(arenaSystem);

    manager.Initialize();

    while (!WindowShouldClose())
    {
        manager.Update();
    }

    manager.Destroy();

    return 1;
}
