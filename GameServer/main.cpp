#include <iostream>
#include <filesystem>
#include "GameServer.h"
#include "CardGenerator.h"

using path = std::filesystem::path;

int main()
{
	std::string log;

#ifndef CIOPILLIS_ROOT
	return 0;
#endif

	path root(CIOPILLIS_ROOT);
	auto db_path = root / "Resources" / "card_database.json";
	if (! exists(db_path))
		return 0;

	CardGenerator generator(db_path.string());

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
