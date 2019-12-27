#pragma once
#include "System.h"
#include <memory>

#ifdef WIN32
#include <thread>
#include <mutex>
#elif __linux__
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#endif

#define WRITE_TYPE 0
#define READ_TYPE 1

class NetworkSystem : public ISystem
{
public:
	NetworkSystem() : ISystem("NetworkSystem") {}
	NetworkSystem(const NetworkSystem& ref) : ISystem("NetworkSystem") {};

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