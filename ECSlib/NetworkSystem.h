#pragma once
#include "System.h"
#include <memory>
#include "Utils.h"

#ifdef WIN32
#include <thread>
#include <mutex>
#include <utility>
#elif __linux__
#include<cstdio>
#include<pthread.h>
#include<cstdlib>
#include<unistd.h>
#endif

#define WRITE_TYPE 0
#define READ_TYPE 1

struct ClientSocket;


using Packet = std::vector<char>;


//The Network system,
//	as a SERVER: receives client sockets and serves them continuously
//	as a CLIENT: connects to a server continuously
//NOTE: this system does not contain a socket from a server, it only receives its
//connections from somewhere else
//THEREFORE: the closing of the server socket must be done BEFORE this system
//calls Destroy()
class INetworkSystem : public ISystem
{
public:

	enum Type
	{
		SERVER,
		CLIENT
	} type;

	std::vector < std::shared_ptr<ClientSocket> > client_sockets;

	std::string server_address;
	
	INetworkSystem(std::string system_name, std::vector<std::shared_ptr<ClientSocket>> client_sockets = {})
		: ISystem(std::move(system_name)), type(SERVER), client_sockets(std::move(client_sockets))
	{}

	INetworkSystem(std::string system_name, std::string server_address)
		: ISystem(std::move(system_name)), type(CLIENT), server_address(server_address)
	{}

#ifdef WIN32
	std::shared_ptr<std::thread> nt = nullptr;
	std::mutex buffer_mutex;
	std::mutex signal_mutex;

	void* socket_ptr = nullptr;
	std::vector<std::shared_ptr<std::thread>> threads;
#elif __linux__
	pthread_t nt;
	pthread_mutex_t buffer_mutex;
	pthread_mutex_t signal_mutex;

	int sd;
	std::vector<pthread_t> threads;
#endif

	bool stop_thread = false;

	bool signal_access(int type, bool value);

	std::vector<Packet> gather_packets();
	void send_packets(std::vector<Packet> packets);
	
	virtual void RunMainThread() = 0;

	void Initialize() override;

	void Destroy() override;
};

class NetworkSystem : public INetworkSystem
{
public:
	/*std::vector<std::array<char, 1024>> write_queue;
	std::vector<std::array<char, 1024>> read_queue;*/

	//FixedQueue<Packet, 40> queue;

	NetworkSystem(std::vector<std::shared_ptr<ClientSocket>> client_sockets = {})
		: INetworkSystem("NetworkSystem",client_sockets)
	{}

	NetworkSystem(std::string server_address)
		: INetworkSystem("NetworkSystem", server_address)
	{}

	char buffer[4096];
	bool changed = false;

	void RunMainThread() override;

	void Execute() override;
	
	void queue_access(int type, char* data);
};