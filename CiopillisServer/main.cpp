#include "GameRoomServer.h"
#include <raylib.h>
#include <ECSlib.h>
#include "Constants.h"
#include "ArenaComponents.h"
#include "ArenaEvents.h"
#include "ArenaSystems.h"

GameRoomManager g;

#ifdef WIN32
void RunGame(std::vector<std::shared_ptr<ClientSocket>> clients)
{
#elif __linux__
void* RunGame(void* param)
{
    auto clients = *(std::vector < std::shared_ptr<ClientSocket>>*)param;
    delete param;
#endif
	//Initialization
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GameRoom");
	SetTargetFPS(60);

	ECSManager manager;

	auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem::SERVER);
	auto drawSystem = std::make_shared<DrawSystem>();
	auto physicsSystem = std::make_shared <PhysicsSystem>();
	auto animationSystem = std::make_shared <AnimationSystem>();
	auto hitBoxSystem = std::make_shared<HitBoxSystem>();
	auto networkSystem = std::make_shared<GameRoomServerSystem>(clients);

	manager.systemManager.AddSystem(drawSystem);
	manager.systemManager.AddSystem(arenaSystem);
	manager.systemManager.AddSystem(physicsSystem);
	manager.systemManager.AddSystem(animationSystem);
	manager.systemManager.AddSystem(hitBoxSystem);
	manager.systemManager.AddSystem(networkSystem);

	auto game(manager.pool.AddEntity());
	game->Add<ArenaGameComponent>();

	manager.eventManager.Subscribe<KeyboardEvent>(arenaSystem);
	manager.eventManager.Subscribe<HitBoxEvent>(arenaSystem);
	manager.eventManager.Subscribe<AnimationEvent>(arenaSystem);
	manager.eventManager.Subscribe<NetworkEvent>(networkSystem);
	manager.eventManager.Subscribe<NetworkEvent>(arenaSystem);
	
	manager.Initialize();

	while (! g.close_server)
	{
		manager.Update();
	}

	manager.Destroy();
}

void* listening_thread(void* param)
{
	g.Initialize();
	
#ifdef WIN32
	std::vector<std::shared_ptr<std::thread>> rooms;
#elif __linux__
    std::vector<pthread_t> rooms;
#endif
	
	while(! g.close_server)
	{
		auto pair = g.get_players();

		if (g.close_server)
			break;

        auto clients = std::vector < std::shared_ptr<ClientSocket>>{ pair.first, pair.second };
#ifdef WIN32
		rooms.push_back(std::make_shared<std::thread>(&RunGame, clients));
#elif __linux__
		pthread_t t;
		auto send_param = new decltype(clients)(clients);
        pthread_create(&t, NULL, &RunGame, send_param);
        rooms.push_back(t);
#endif	
	}

#ifdef WIN32
	for (auto& t : rooms)
	{
		t->join();
	}
#elif __linux__
    for (auto& t : rooms)
    {
        pthread_join(t, NULL);
    }
#endif

    return nullptr;
}

int main()
{
	InitWindow(100, 100, "Ciopillis Server");

#ifdef WIN32
	std::thread listening(listening_thread, nullptr);
#elif __linux__
	pthread_t listening;
	pthread_create(&listening, NULL, &listening_thread, nullptr);
#endif

	while(! WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	g.Destroy();

#ifdef WIN32
    listening.join();
#elif __linux__
    pthread_join(listening, NULL);
#endif
}