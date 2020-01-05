#include "GameRoomServer.h"
#include <raylib.h>
#include <ECSlib.h>
#include "Constants.h"
#include "ArenaComponents.h"
#include "ArenaEvents.h"
#include "ArenaSystems.h"

GameRoomManager g;

void RunGame(std::vector<std::shared_ptr<ClientSocket>> clients)
{
	//Initialization
	SetTargetFPS(60);

	ECSManager manager;

	//auto defferSystem = std::make_shared<DefferSystem>();
	//auto arenaSystem = std::make_shared<ArenaSystem>();
	//auto arenaEventSystem = std::make_shared<ArenaEventSystem>();
	//auto drawSystem = std::make_shared<DrawSystem>();
	//auto keyboardInputSystem = std::make_shared<KeyboardInputSystem>();
	//auto physicsSystem = std::make_shared <PhysicsSystem>();
	//auto animationSystem = std::make_shared <AnimationSystem>();
	//auto hitBoxSystem = std::make_shared<HitBoxSystem>();
	auto networkSystem = std::make_shared<NetworkSystem>(clients);

	//manager.systemManager.AddSystem(defferSystem);
	//manager.systemManager.AddSystem(drawSystem);
	//manager.systemManager.AddSystem(arenaSystem);
	//manager.systemManager.AddSystem(arenaEventSystem);
	//manager.systemManager.AddSystem(keyboardInputSystem);
	//manager.systemManager.AddSystem(physicsSystem);
	//manager.systemManager.AddSystem(animationSystem);
	//manager.systemManager.AddSystem(hitBoxSystem);
	manager.systemManager.AddSystem(networkSystem);

	//auto game(manager.pool.AddEntity());
	//game->Add<ArenaGameComponent>();

	//manager.eventManager.Subscribe<DefferEvent>(defferSystem);
	//manager.eventManager.Subscribe<KeyboardEvent>(arenaEventSystem);
	//manager.eventManager.Subscribe<ArenaPlayerEvent>(arenaSystem);
	//manager.eventManager.Subscribe<HitBoxEvent>(arenaSystem);

	manager.Initialize();

	while (! g.close_server)
	{
		manager.Update();
	}

	manager.Destroy();
}

void listening_thread()
{
	g.Initialize();
	
#ifdef WIN32
	std::vector<std::shared_ptr<std::thread>> rooms;
#elif __linux__

#endif
	
	while(! g.close_server)
	{
		auto pair = g.get_players();

		if (g.close_server)
			break;
		
#ifdef WIN32		
		auto clients = std::vector < std::shared_ptr<ClientSocket>>{ pair.first, pair.second };
		rooms.push_back(std::make_shared<std::thread>(&RunGame, clients));
#elif __linux__

#endif	
	}

#ifdef WIN32
	for (auto& t : rooms)
	{
		t->join();
	}
#elif __linux__
	pthread_join(nt, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&signal_mutex);
#endif
}

int main()
{
	InitWindow(100, 100, "Ciopillis Server");

#ifdef WIN32
	std::thread listening(listening_thread);
#elif __linux__
#endif

	while(! WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	g.Destroy();

	listening.join();
}