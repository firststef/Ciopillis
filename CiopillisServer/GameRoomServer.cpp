#include "GameRoomServer.h"
#include "Constants.h"
#include <iostream>
#ifdef WIN32
#include <WS2tcpip.h>
#elif __linux__
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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
		std::cerr << "Can't Initialize winsock! Quitting" << std::endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting" << std::endl;
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
        return;
    }

    if (listen (sd, 2) == -1)
    {
        perror ("Error on listen.\n");
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

	auto client2 = std::make_shared<ClientSocket>();
	client2->clientSize = sizeof(client1->address);
	client2->clientSocket = accept(server_socket, (sockaddr*)&(client1->address), &(client1->clientSize));

	std::cout << "client2 connected\n" << WSAGetLastError() << "\n";

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

    printf ("[server]Asteptam la portul %d...\n",54000);
    fflush (stdout);

    if ( (client1->cl = accept (sd, (struct sockaddr *) &from1, &length1)) < 0)
    {
        perror ("[server]Eroare la accept().\n");
    }

    sockaddr_in from2;
    bzero (&from2, sizeof (from2));
    socklen_t length2 = sizeof (from2);
    auto client2 = std::make_shared<ClientSocket>();

    printf ("[server]Asteptam la portul %d...\n",54000);
    fflush (stdout);

    if ( (client1->cl = accept (sd, (struct sockaddr *) &from2, &length2)) < 0)
    {
        perror ("[server]Eroare la accept().\n");
    }

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
