#include "GameServer.h"
#include "CardGenerator.h"
#include <iostream>

int main()
{
    std::string log;

    auto player = Player("Player");
    auto computer = Player("Computer");

    CardGenerator generator("card_database.json");

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
