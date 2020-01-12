#include "NetworkSystem.h"
#include <deque>

//class GameRoomClientSystem : INetworkSystem
//{
//public:
//
//	std::deque<std::array<char, 4096>> queue;
//
//	GameRoomClientSystem(std::string server_address, int port)
//		:INetworkSystem("GameRoomClientSystem", server_address, port)
//	{}
//
//	void queue_access(AccessType type, void* context);
//
//	void RunMainThread() override;
//
//	void Execute() override;
//};