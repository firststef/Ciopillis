#include "GameServer.h"
#include "CardGenerator.h"
#include <iostream>

int main()
{
	std::string log;

#ifdef WIN32
    CardGenerator generator("D:/GameDev/Ciopillis/GameServer/card_database.json");
#else
	CardGenerator generator("/home/first/Documents/Ciopillis/GameServer/card_database.json");
#endif

    auto player = Player("Player", generator.container);
    auto computer = Player("Computer",generator.container);

    GameServer server(GameServer::Interface::CONSOLE, log, generator.container, player, computer);

    while(true)
    {
        system("CLS");
        std::cout<<log;
        if (!server.RunConsole())
            break;
    }

    system("pause");
    return 1;
}
