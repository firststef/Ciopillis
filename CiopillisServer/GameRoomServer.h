#pragma once
#include "NetworkSystem.h"

class GameRoomManager
{
public:
	
#ifdef WIN32
	unsigned int server_socket;
#elif __linux__ 
	int sd;
#endif

	bool close_server = false;

	void Initialize();

	std::pair<std::shared_ptr<ClientSocket>, std::shared_ptr<ClientSocket>> get_players();

	void Destroy();
};

class GameRoomServerSystem : public INetworkSystem
{
public:

	GameRoomServerSystem(std::vector<std::shared_ptr<ClientSocket>> client_sockets = {})
		: INetworkSystem("GameRoomServerSystem", client_sockets)
	{}

	GameRoomServerSystem(std::string server_address, int port)
		: INetworkSystem("GameRoomServerSystem", server_address, port)
	{}

	std::pair<std::vector<Packet>, std::vector<Packet>> gather_p();
	void send_p(std::pair<std::vector<Packet>, std::vector<Packet>>);

	void RunMainThread() override;

	void Execute() override;
};