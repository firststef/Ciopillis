#include "NetworkSystem.h"
#include <iostream>
#include "NetworkEvent.h"
#include "EventManager.h"
#include "External.h"
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

bool INetworkSystem::signal_access(AccessType type, bool value)
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
	if (type == CLIENT) {
#ifdef WIN32
		// Initialize WinSock
		WSAData data;
		WORD ver = MAKEWORD(2, 2);
		int wsResult = WSAStartup(ver, &data);
		if (wsResult != 0)
		{
			std::cout << "Can't start Winsock, Err #" << wsResult << std::endl;
			signal_access(WRITE_TYPE, true);
			return;
		}

		// Create socket
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
		{
			std::cout << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
			signal_access(WRITE_TYPE, true);
			return;
		}

		socket_ptr = sock;

		// Fill in a hint structure
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, server_address.c_str(), &hint.sin_addr);

		// Connect to server
		int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
		if (connResult == SOCKET_ERROR)
		{
			std::cout << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
			closesocket(sock);
			WSACleanup();
			signal_access(WRITE_TYPE, true);
			return;
		}
#elif __linux__
		sockaddr_in server;

		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("Error on socket().\n");
			signal_access(WRITE_TYPE, true);
			return;
		}

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(server_address.c_str());
		server.sin_port = htons(port);

		if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
		{
			perror("Error on connect().\n");
			signal_access(WRITE_TYPE, true);
			return;
		}
#endif
	}

#ifdef WIN32
	nt = std::make_shared<std::thread>(&INetworkSystem::RunMainThread, this);
#elif __linux__
	if (pthread_mutex_init(&buffer_mutex, NULL) != 0 || pthread_mutex_init(&signal_mutex, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		signal_access(WRITE_TYPE, true);
		return;
	}
	
    int err = pthread_create(&nt, NULL, &main_thread_f_wrapper, (void*)this);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
#endif
    std::cout << "Thread started" << std::endl;
}

std::vector<Packet> INetworkSystem::gather_packets()
{
    std::vector<Packet> packets;
    if (type == SERVER) {

        for (auto &client : client_sockets) {
            char buffer[4096];

            int bytesReceived;
#ifdef WIN32
            ZeroMemory(buffer, 4096);

            // Wait for client to send data
            bytesReceived = recv(client->clientSocket, buffer, 4096, 0);
            if (bytesReceived <= 0 )
            {
                std::cout << "Error in recv(). Quitting" << std::endl;
				signal_access(WRITE_TYPE, true);
                break;
            }

#elif __linux__
            bytesReceived = read(client->cl, buffer, 4096);

            if (bytesReceived <= 0) {
                perror("Error in read(). Quitting.\n");
                signal_access(WRITE_TYPE, true);
            }
#endif
            if (signal_access(READ_TYPE, false))
                break;

            std::vector<char> packet;
            packet.insert(packet.begin(), buffer, buffer + bytesReceived);
            packets.push_back(packet);
        }
    }
    else {
        char buffer[4096];

        int bytesReceived;
#ifdef WIN32
		ZeroMemory(buffer, 4096);
		bytesReceived = recv(socket_ptr, buffer, 4096, 0);
		if(bytesReceived <= 0)
		{
			std::cout << "Error on recv(). Quitting\n";
			signal_access(WRITE_TYPE, true);
			return packets;
		}
    	
#elif __linux__
        bytesReceived = read(sd, buffer, 4096);

        if (bytesReceived <= 0) {
            perror("Error in read(). Quitting.\n");
            signal_access(WRITE_TYPE, true);

			return packets;
        }
#endif
		std::vector<char> packet;
		packet.insert(packet.begin(), buffer, buffer + bytesReceived);
		packets.push_back(packet);
    }

	return packets;
}

void INetworkSystem::send_packets(std::vector<Packet> packets)
{
    if (type == SERVER) {
        if (client_sockets.size() != packets.size()) {
            packets.resize(client_sockets.size());
        	for (auto& p: packets)
        	{
				if (p.empty())
					p.resize(1);
        	}
        }

        for (unsigned int i = 0; i < client_sockets.size(); ++i) {
#ifdef WIN32
            if (send(client_sockets[i]->clientSocket, &packets[i][0], packets[i].size() + 1, 0) <= 0)
            {
				signal_access(WRITE_TYPE, true);
            }
#elif __linux__
            if (write(client_sockets[i]->cl, &packets[i][0], packets[i].size() + 1) <= 0) {
                perror("Error in write(). Quitting.\n");
                signal_access(WRITE_TYPE, true);
            }
#endif
        }
    }
    else {
		if (packets.empty())
			packets.resize(1);
		for (auto& p : packets)
		{
			if (p.empty())
				p.resize(1);
		}
    	
#ifdef WIN32
		int sendResult = send(socket_ptr, &packets[0][0], packets[0].size() + 1, 0);
		if (sendResult <= 0)
		{
			std::cout << "Error on send(). Quitting\n" << WSAGetLastError() << "\n";
			signal_access(WRITE_TYPE, true);
		}
#elif __linux__
        if (write(sd, &packets[0][0], packets[0].size() + 1) <= 0) {
            perror("Error in write(). Quitting.\n");
            signal_access(WRITE_TYPE, true);
        }
#endif
    }
}

//NOTE: this must be closed after the listening socket is closed
void INetworkSystem::Destroy()
{
	signal_access(WRITE_TYPE, true);
	if (type == SERVER) {
#ifdef WIN32
		for (auto &c : client_sockets) {
			closesocket(c->clientSocket);
		}
#elif __linux__
		for (auto &c : client_sockets) {
			shutdown(c->cl, 2);
			close(c->cl);
			c->cl = -1;
		}
#endif
	}
	else {
#ifdef WIN32
		closesocket(socket_ptr);
#elif __linux__
		close(sd);
#endif
	}

#ifdef WIN32
	nt->join();
#elif __linux__
	pthread_join(nt, NULL);
	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&signal_mutex);
#endif
}

void NetworkSystem::RunMainThread()
{
    std::cout << "Running Main Thread" << std::endl;
	if (type == SERVER) {
		while(true)
		{
			if (signal_access(READ_TYPE, false))
				break;

			auto packets = gather_packets();

            if (signal_access(READ_TYPE, false))
                break;

			Packet new_packet;
			for (auto& p : packets)
			{
				new_packet.insert(new_packet.end(), p.begin(), p.end());
			}
			receive_queue_access(WRITE_TYPE, &new_packet);

			std::vector<Packet> new_packets;
			for (auto& client : client_sockets) {
				Packet pack;
				for (auto& packet : packets)
				{
					pack.insert(pack.end(), packet.begin(), packet.end());
				}
				new_packets.push_back(pack);
			}

			send_packets(new_packets);
		}
	}
	else
	{
	    while(true) {

            if (signal_access(READ_TYPE, false))
                break;

			auto send = send_queue_access(READ_TYPE, nullptr);
			send_packets(send);

			SleepFunc(10);

            auto p = gather_packets();

            if (signal_access(READ_TYPE, false))
                break;

			if (p.empty())
				break;

	    	for(auto& pack : p)
	    	{
				receive_queue_access(WRITE_TYPE, &pack);
	    	}
        }
	}
}

void NetworkSystem::Execute()
{
	auto packets = receive_queue_access(READ_TYPE, nullptr);
	if (!packets.empty())
		eventManager->Notify<NetworkEvent>(NetworkEvent::RECEIVE, packets);
}

std::vector<Packet> NetworkSystem::send_queue_access(AccessType type, const Packet* packet)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(buffer_mutex);
#else
	pthread_mutex_lock(&buffer_mutex);
#endif

	std::vector<Packet> packets;
	const Packet& data = *packet;
	
	if (type == WRITE_TYPE)
	{
		/*struct tm * dt;
		char buffer[30];
		time_t rawtime = time(nullptr);
		dt = localtime(&rawtime);
		strftime(buffer, sizeof(buffer), "Send queue put %m%d%H%M%y:", dt);
		std::cout << std::string(buffer) << std::endl;*/
		
		if (send_queue.is_full())
		{
			std::cout << "Warning: Send queue full, dropping data\n";
		}
		send_queue.push(data);
	}
	else
	{
		while(! send_queue.empty())
		{
			packets.push_back(send_queue.front());
			send_queue.pop();
		}
	}

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif

	return packets;
}

std::vector<Packet> NetworkSystem::receive_queue_access(AccessType type, const Packet* packet)
{
#ifdef WIN32
	std::lock_guard<std::mutex> guard(buffer_mutex);
#else
	pthread_mutex_lock(&buffer_mutex);
#endif

	std::vector<Packet> packets;
	const Packet& data = *packet;

	if (type == WRITE_TYPE)
	{
		if (receive_queue.is_full())
		{
			std::cout << "Warning: Receive queue full, dropping data\n";
		}
		receive_queue.push(data);
	}
	else
	{
		while (!receive_queue.empty())
		{
			packets.push_back(receive_queue.front());
			receive_queue.pop();
		}
	}

#ifdef __linux__
	pthread_mutex_unlock(&buffer_mutex);
#endif

	return packets;
}

void NetworkSystem::Receive(const NetworkEvent& event)
{
	if (event.type == NetworkEvent::SEND)
	{
		for (auto& p : event.packets)
		{
			send_queue_access(WRITE_TYPE, &p);
		}
	}
}
