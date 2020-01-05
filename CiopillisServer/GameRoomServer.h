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