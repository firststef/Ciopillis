#include <raylib.h>
#include <ECSlib.h>
#include "Constants.h"
#include "ArenaComponents.h"
#include "ArenaEvents.h"
#include "ArenaSystems.h"

int main(int argc, char** argv)
{
	std::string server_ip;
	if (argc > 1)
		server_ip = argv[1];
	else
		server_ip = "127.0.0.1";
	
	//Initialization
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Client");
	SetTargetFPS(60);

	ECSManager manager;

	auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem::CLIENT);
	auto drawSystem = std::make_shared<DrawSystem>();
	auto keyboardInputSystem = std::make_shared<KeyboardInputSystem>();
	auto physicsSystem = std::make_shared <PhysicsSystem>();
	auto animationSystem = std::make_shared <AnimationSystem>();
	auto hitBoxSystem = std::make_shared<HitBoxSystem>();
	auto networkSystem = std::make_shared<NetworkSystem>(server_ip, 54000);

	manager.systemManager.AddSystem(drawSystem);
	manager.systemManager.AddSystem(arenaSystem);
	manager.systemManager.AddSystem(keyboardInputSystem, false);
	manager.systemManager.AddSystem(physicsSystem, false);
	manager.systemManager.AddSystem(animationSystem, false);
	manager.systemManager.AddSystem(hitBoxSystem, false);
	manager.systemManager.AddSystem(networkSystem);

	auto game(manager.pool.AddEntity());
	game->Add<ArenaGameComponent>();

	manager.eventManager.Subscribe<KeyboardEvent>(arenaSystem);
	manager.eventManager.Subscribe<HitBoxEvent>(arenaSystem);
	manager.eventManager.Subscribe<AnimationEvent>(arenaSystem);
	manager.eventManager.Subscribe<NetworkEvent>(networkSystem);
	manager.eventManager.Subscribe<NetworkEvent>(arenaSystem);
	manager.eventManager.Subscribe<SystemControlEvent>(keyboardInputSystem);
	manager.eventManager.Subscribe<SystemControlEvent>(physicsSystem);
	manager.eventManager.Subscribe<SystemControlEvent>(animationSystem);
	manager.eventManager.Subscribe<SystemControlEvent>(hitBoxSystem);

	manager.Initialize();

	while (!WindowShouldClose())
	{
		manager.Update();
	}

	manager.Destroy();

	return 1;
}
