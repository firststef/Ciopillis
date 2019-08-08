// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//ok deci clar operatiile functiilor vor fi executate de o masina virtuala

#include "GameServer.h"
#include "CardGenerator.h"
#include <iostream>

int main()
{
    std::string log;

    CardGenerator generator("card_database.json");

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
