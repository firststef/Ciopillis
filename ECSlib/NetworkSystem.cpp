#include "NetworkSystem.h"
#include <iostream>
#ifdef WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

void NetworkSystem::buffer_access(int type, char* data)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(buffer_mutex);
#else
	pthread_mutex_lock(&buffer_mutex);
#endif

	if (type == WRITE_TYPE)
	{
		memset(buffer, 0, 1024);
		memcpy(buffer, data, 1024);
		changed = true;
	}
	else
	{
		if (changed)
		{
			std::cout << buffer << std::endl;
			changed = false;
		}
	}

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif
}

bool NetworkSystem::signal_access(int type, bool value)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(signal_mutex);
#else
	pthread_mutex_lock(&signal_mutex);
#endif

	if (type == WRITE_TYPE)
	{
		stop_thread = value;
		return false;
	}

	return stop_thread;

#ifdef __linux__
	pthread_mutex_unlock(&signal_mutex);
#endif
}

void NetworkSystem::terminate_socket()
{
#ifdef WIN32
    closesocket(*(SOCKET*)socket_ptr);
	if (socket_ptr)
	{
		socket_ptr = nullptr;
	}
#elif __linux__
	close(sd);
#endif
}

void NetworkSystem::ThreadRun()
{
#ifdef WIN32
	WSADATA data;

	WORD version = MAKEWORD(2, 2);

	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		std::cout << "Can't start Winsock! " << wsOk;
		return;
	}

	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);
	socket_ptr = &in;

	sockaddr_in serverHint;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(54000); // Convert from little to big endian

	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return;
	}

	sockaddr_in client;
	int clientLength = sizeof(client);

	char buf[1024];

	while (true)
	{
		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, 1024); // Clear the receive buffer

		// Wait for message
		int bytesIn = recvfrom(in, buf, 1024, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from client " << WSAGetLastError() << std::endl;
			break;
		}

		char clientIp[256];
		ZeroMemory(clientIp, 256);

		// Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		std::cout << "Message recv from " << clientIp << " : " << buf << std::endl;
	}

	WSACleanup();
#elif __linux__
    sockaddr_in server;
    char msg[1024];
    int msglen=0;
    socklen_t length=0;

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons (54000);

    while(true) {
        bzero(msg, 100);
        strcpy(msg, "test message\n");

        length = sizeof(server);
        if (sendto(sd, msg, 100, 0, (sockaddr *) &server, length) <= 0) {
            //perror("[client]Eroare la sendto() spre server.\n");
            break;
        }

        sleep(5);
    }
#endif
}

void* function_wrapper(void* ptr)
{
	std::bind(&NetworkSystem::ThreadRun, (NetworkSystem*)ptr)();
    return nullptr;
}

void NetworkSystem::Initialize()
{
#ifdef WIN32
	nt = std::make_shared<std::thread>(&NetworkSystem::ThreadRun, this);
	std::cout << "Thread started" << std::endl;
#elif __linux__
	if (pthread_mutex_init(&buffer_mutex, NULL) != 0 || pthread_mutex_init(&signal_mutex, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return;
	}
	
	int err = pthread_create(&nt, NULL, &function_wrapper, (void*)this);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
#endif
}

void NetworkSystem::Execute()
{
	buffer_access(READ_TYPE, nullptr);
}

void NetworkSystem::Destroy()
{
	terminate_socket();
#ifdef WIN32
	nt->join();
#elif __linux__
	pthread_join(nt, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&signal_mutex);
#endif
}
