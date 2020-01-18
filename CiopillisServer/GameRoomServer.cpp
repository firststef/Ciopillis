#include "GameRoomServer.h"
#include "Constants.h"
#include "External.h"
#include <iostream>
#ifdef WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
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

void GameRoomManager::Initialize()
{
#ifdef WIN32
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cout << "Can't Initialize winsock! Quitting" << std::endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cout << "Can't create a socket! Quitting" << std::endl;
		return;
	}

	server_socket = listening;

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);
#elif __linux__
    sockaddr_in server;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Error creating socket.\n");
        close_server = true;
        return;
    }

    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    bzero (&server, sizeof (server));

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = htonl (INADDR_ANY);

    server.sin_port = htons (PORT);

    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("Error binding socket.\n");
        close_server = true;
        return;
    }

    if (listen (sd, 2) == -1)
    {
        perror ("Error on listen.\n");
        close_server = true;
        return;
    }
#endif
}

std::pair<std::shared_ptr<ClientSocket>, std::shared_ptr<ClientSocket>> GameRoomManager::get_players()
{
#ifdef WIN32

	auto client1 = std::make_shared<ClientSocket>();
	client1->clientSize = sizeof(client1->address);
	client1->clientSocket = accept(server_socket, (sockaddr*)&(client1->address), &(client1->clientSize));

	std::cout << "client1 connected\n" << WSAGetLastError() << "\n";

	u_long iMode1 = 1;
	auto iResult = ioctlsocket(client1->clientSocket, FIONBIO, &iMode1);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	auto client2 = std::make_shared<ClientSocket>();
	client2->clientSize = sizeof(client1->address);
	client2->clientSocket = accept(server_socket, (sockaddr*)&(client1->address), &(client1->clientSize));

	std::cout << "client2 connected\n" << WSAGetLastError() << "\n";

	u_long iMode2 = 1;
	iResult = ioctlsocket(client2->clientSocket, FIONBIO, &iMode2);
	if (iResult != NO_ERROR)
		printf("ioctlsocket failed with error: %ld\n", iResult);

	char client1_host[NI_MAXHOST];		// Client's remote name
	char client1_service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(client1_host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(client1_service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&(client1->address), sizeof(client1->address), client1_host, NI_MAXHOST, client1_service, NI_MAXSERV, 0) == 0)
	{
		std::cout << client1_host << " connected on port " << client1_service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &(client1->address.sin_addr), client1_host, NI_MAXHOST);
		std::cout << client1_host << " connected on port " <<
			ntohs(client1->address.sin_port) << std::endl;
	}

	char client2_host[NI_MAXHOST];		// Client's remote name
	char client2_service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(client2_host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(client2_service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&(client2->address), sizeof(client2->address), client2_host, NI_MAXHOST, client2_service, NI_MAXSERV, 0) == 0)
	{
		std::cout << client2_host << " connected on port " << client2_service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &(client2->address.sin_addr), client2_host, NI_MAXHOST);
		std::cout << client2_host << " connected on port " <<
			ntohs(client2->address.sin_port) << std::endl;
	}

	return std::pair<std::shared_ptr<ClientSocket>, std::shared_ptr<ClientSocket>>(client1, client2);
	
#elif __linux__

    sockaddr_in from1;
    bzero (&from1, sizeof (from1));
    socklen_t length1 = sizeof (from1);
    auto client1 = std::make_shared<ClientSocket>();

    printf ("[server]Waiting on port %d...\n",54000);
    fflush (stdout);

    if ( (client1->cl = accept (sd, (struct sockaddr *) &from1, &length1)) < 0)
    {
        perror ("[server]Eroare la accept().\n");
        close_server = true;
    }

	int flags = fcntl(client1->cl, F_GETFL);
	fcntl(client1->cl, F_SETFL, flags | O_NONBLOCK);

    sockaddr_in from2;
    bzero (&from2, sizeof (from2));
    socklen_t length2 = sizeof (from2);
    auto client2 = std::make_shared<ClientSocket>();

    printf ("[server]Waiting on port %d...\n",54000);
    fflush (stdout);

    if ( (client2->cl = accept (sd, (struct sockaddr *) &from2, &length2)) < 0)
    {
        perror ("[server]Eroare la accept().\n");
        close_server = true;
    }

	flags = fcntl(client2->cl, F_GETFL);
	fcntl(client2->cl, F_SETFL, flags | O_NONBLOCK);

    return std::pair<std::shared_ptr<ClientSocket>, std::shared_ptr<ClientSocket>>(client1, client2);

#endif
}

void GameRoomManager::Destroy()
{
	close_server = true;
#ifdef WIN32
	if (server_socket != 0)
	{
		closesocket(server_socket);
		server_socket = 0;
	}

	WSACleanup();
#elif __linux__
	shutdown(sd, 2);
	close(sd);
	sd = -1;
#endif
}

void GameRoomServerSystem::RunMainThread()
{
	while (true)
	{
		auto packets = gather_packets();

		if (signal_access(READ_TYPE, false))
			break;

		for (auto& pack : packets[0])
		{
			//printf("packet from 0\n");
			
			if (pack.empty())
				continue;

			if (strcmp(pack.data(), "connect") == 0)
			{
				continue;
			}

			std::string jstr(&pack[0], pack.size() + 1);
			jstr[pack.size()] = '\0';
			nlohmann::json j = nlohmann::json::parse(jstr.c_str(), nullptr, false);
			if (j.is_discarded())
			{
				printf("Malformed packet%d\n", __LINE__);
				continue;
			}

			j.emplace("player", 0);
			auto str = j.dump();
			Packet p;
			p.insert(p.begin(), str.begin(), str.end() + 1);
			
			receive_queue_access(WRITE_TYPE, &p);
		}

		for (auto& pack : packets[1])
		{
			//printf("packet from 1\n");
			
			if (pack.empty())
				continue;

			if (strcmp(pack.data(), "connect") == 0)
			{
				continue;
			}

			std::string jstr(&pack[0], pack.size() + 1);
			jstr[pack.size()] = '\0';
			nlohmann::json j = nlohmann::json::parse(jstr.c_str(), nullptr, false);
			if (j.is_discarded())
			{
				printf("Malformed packet%d\n", __LINE__);
				continue;
			}

			j.emplace("enemy", 0);
			auto str = j.dump();
			Packet p;
			p.insert(p.begin(), str.begin(), str.end()+1);

			receive_queue_access(WRITE_TYPE, &p);
		}

		auto send = send_queue_access(READ_TYPE, nullptr);

		SleepFunc(9);

		std::vector<Packet> packets_for_one;
		std::vector<Packet> packets_for_two;
		for (auto& pack : send)
		{
			if (pack.empty())
				continue;

			std::string jstr(&pack[0], pack.size() + 1);
			jstr[pack.size()] = '\0';
			nlohmann::json j = nlohmann::json::parse(jstr.c_str(), nullptr, false);
			if (j.is_discarded())
			{
				printf("Malformed packet%d\n", __LINE__);
				continue;
			}
			
			packets_for_one.push_back(pack);

			auto b = j.find("player") != j.end();

			nlohmann::json::iterator it1 = j.find("player");
			std::swap(j["aux"], it1.value());
			j.erase(it1);

			nlohmann::json::iterator it2 = j.find("enemy");
			std::swap(j["player"], it2.value());
			j.erase(it2);

			nlohmann::json::iterator it3 = j.find("aux");
			std::swap(j["enemy"], it3.value());
			j.erase(it3);
			
			auto str = j.dump();
			Packet p;
			p.insert(p.begin(), str.begin(), str.end() + 1);
			packets_for_two.push_back(p);
		}

		if (!packets_for_one.empty())
		{
			auto str = packets_for_one[0].data();
			auto x = 0;
		}

		if (!packets_for_two.empty())
		{
			auto str = packets_for_two[0].data();
			auto x = 0;
		}
		
		send_packets({ packets_for_one, packets_for_two });

		if (signal_access(READ_TYPE, false))
			break;
	}
	
}