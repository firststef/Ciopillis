#pragma once
#include "System.h"
#include <memory>

#ifdef WIN32
#include <thread>
#include <mutex>
#elif __linux__
#include<cstdio>
#include<pthread.h>
#include<cstdlib>
#include<unistd.h>
#endif

#define WRITE_TYPE 0
#define READ_TYPE 1

class NetworkSystem : public ISystem
{
public:

	enum Type
	{
		SERVER,
		CLIENT
	} type;
	
	NetworkSystem(Type type) : ISystem("NetworkSystem"), type(type) {}

#ifdef WIN32
	std::shared_ptr<std::thread> nt = nullptr;
	std::mutex buffer_mutex;
	std::mutex signal_mutex;

	void* socket_ptr = nullptr;
#elif __linux__ 
	pthread_t nt;
	pthread_mutex_t buffer_mutex;
	pthread_mutex_t signal_mutex;

	int sd;
#endif

	char buffer[1024];
	bool changed = false;

	bool stop_thread = false;

	void buffer_access(int type, char* data);

	bool signal_access(int type, bool value);

	void terminate_socket();

	void ThreadRun();

	void Initialize() override;

	void Execute() override;

	void Destroy() override;
};