#include "NetworkSystem.h"
#include <iostream>
#ifdef WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

struct ClientSocket
{
#ifdef WIN32
	sockaddr_in address;
	int clientSize;
	SOCKET clientSocket;
#elif __linux__
    int cl;
#endif
};

bool INetworkSystem::signal_access(int type, bool value)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(signal_mutex);
#else
	pthread_mutex_lock(&signal_mutex);
#endif

	if (type == WRITE_TYPE)
	{
		stop_thread = value;
	}

#ifdef __linux__
	pthread_mutex_unlock(&signal_mutex);
#endif

	return stop_thread;
}

void* main_thread_f_wrapper(void* ptr)
{
    std::bind(&INetworkSystem::RunMainThread, (INetworkSystem*)ptr)();
    return nullptr;
}

void INetworkSystem::Initialize()
{
#ifdef WIN32
    nt = std::make_shared<std::thread>(&INetworkSystem::RunMainThread, this);
#elif __linux__
    if (pthread_mutex_init(&buffer_mutex, NULL) != 0 || pthread_mutex_init(&signal_mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return;
    }

    int err = pthread_create(&nt, NULL, &main_thread_f_wrapper, (void*)this);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
#endif
    std::cout << "Thread started" << std::endl;
}

//NOTE: this must be closed after the listening socket is closed
void INetworkSystem::Destroy()
{
	signal_access(WRITE_TYPE, true);
#ifdef WIN32
	for (auto& t : threads)
	{
		t->join();
	}

	nt->join();
#elif __linux__
    for (auto& t : threads)
    {
        pthread_join(t, NULL);
    }

	pthread_join(nt, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&signal_mutex);
#endif
}

std::vector<Packet> INetworkSystem::gather_packets()
{
	std::vector<Packet> packets;

	for (auto& client : client_sockets)
	{
		char buffer[4096];

		int bytesReceived;
#ifdef WIN32
		ZeroMemory(buffer, 4096);

		// Wait for client to send data
		bytesReceived = recv(client->clientSocket, buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error1 in recv(). Quitting" << std::endl;//validari
			break;
		}

		if (bytesReceived == 0)
		{
			std::cout << "Client1 disconnected " << std::endl;//aici trebuie facut event
			break;
		}

#elif __linux__

#endif
		std::vector<char> packet;
		packet.insert(packet.begin(), buffer, buffer + bytesReceived);
		packets.push_back(packet);
	}

	return packets;
}

void INetworkSystem::send_packets(std::vector<Packet> packets)
{
	if (client_sockets.size() != packets.size())
	{
		packets.resize(client_sockets.size());
	}

	for (unsigned int i = 0; i < client_sockets.size(); ++i)
	{
#ifdef WIN32
		send(client_sockets[i]->clientSocket, &packets[i][0], packets[i].size() + 1, 0);
#elif __linux__

#endif
	}

	printf("sent all packets\n");
}

void NetworkSystem::RunMainThread()
{
	if (type == SERVER) {
	    std::cout << "Running" << std::endl;
#ifdef WIN32
		while(true)
		{
			if (signal_access(READ_TYPE, false))
				break;

			auto packets = gather_packets();

			char buffer[4096];
			memset(buffer, '\0', 4096);
			for (auto& p : packets)
			{
				strcpy(buffer + strlen(buffer), &p[0]);
			}
			queue_access(WRITE_TYPE, buffer);

			std::vector<Packet> new_packets;
			for (auto& client : client_sockets) {
				Packet new_packet;
				for (auto& packet : packets)
				{
					new_packet.insert(new_packet.end(), packet.begin(), packet.end());
				}
				new_packets.push_back(new_packet);
			}

			send_packets(new_packets);
		}
#elif __linux__



        while (1)
        {


            if (signal_access(READ_TYPE, false))
                break;

        }
#endif
	}
	else
	{
#ifdef WIN32
		
#elif __linux__
//		sockaddr_in server;
//		char msg[1024];
//		int msglen = 0;
//		socklen_t length = 0;
//
//		/* cream socketul */
//		if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
//		{
//			perror("Eroare la socket().\n");
//			return;
//		}
//
//		server.sin_family = AF_INET;
//		server.sin_addr.s_addr = inet_addr("127.0.0.1");
//		server.sin_port = htons(54000);
//
//		while (true) {
//			bzero(msg, 100);
//			strcpy(msg, "test message\n");
//
//			length = sizeof(server);
//			if (sendto(sd, msg, 100, 0, (sockaddr *)&server, length) <= 0) {
//				//perror("[client]Eroare la sendto() spre server.\n");
//				break;
//			}
//
//			if (signal_access(READ_TYPE, false))
//				break;
//
//			sleep(5);
//		}
//
//        close(sd);
#endif
	}
}

void NetworkSystem::Execute()
{
	queue_access(READ_TYPE, nullptr);
}

void NetworkSystem::queue_access(int type, char* data)
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

	/*	if (type == WRITE_TYPE)
	{
		std::array<char, 1024> buffer;
		memset(&buffer, 0, 1024);
		memcpy(&buffer, data, 1024);

		write_queue.push_back(buffer);
	}
	else
	{
		for (auto& b : write_queue)
		{
			read_queue.push_back(b);
		}
		write_queue.clear();
	}*/

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif
}