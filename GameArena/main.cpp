#include <raylib.h>
#include <ECSlib.h>
#include "Constants.h"
#include "ArenaComponents.h"
#include "ArenaEvents.h"
#include "ArenaSystems.h"

int main()
{
    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    ECSManager manager;

	auto defferSystem = std::make_shared<DefferSystem>();
	auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem::TESTER);
	auto drawSystem = std::make_shared<DrawSystem>();
	auto keyboardInputSystem = std::make_shared<KeyboardInputSystem>();
	auto physicsSystem = std::make_shared <PhysicsSystem>();
	auto animationSystem = std::make_shared <AnimationSystem>();
	auto hitBoxSystem = std::make_shared<HitBoxSystem>();
	//auto networkSystem = std::make_shared<INetworkSystem>(INetworkSystem::CLIENT);

	manager.systemManager.AddSystem(defferSystem);
	manager.systemManager.AddSystem(drawSystem);
	manager.systemManager.AddSystem(arenaSystem);
	manager.systemManager.AddSystem(keyboardInputSystem);
	manager.systemManager.AddSystem(physicsSystem);
	manager.systemManager.AddSystem(animationSystem);
	manager.systemManager.AddSystem(hitBoxSystem);
	//manager.systemManager.AddSystem(networkSystem);

	auto game(manager.pool.AddEntity());
	game->Add<ArenaGameComponent>();

	manager.eventManager.Subscribe<DefferEvent>(defferSystem);
	manager.eventManager.Subscribe<KeyboardEvent>(arenaSystem);
	manager.eventManager.Subscribe<HitBoxEvent>(arenaSystem);
	manager.eventManager.Subscribe<AnimationEvent>(arenaSystem);

    manager.Initialize();

    while (!WindowShouldClose())
    {
        manager.Update();
    }

    manager.Destroy();

    return 1;
}
