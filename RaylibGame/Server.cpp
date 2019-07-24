#include "Server.h"
#include <iostream>

GameServer::GameServer(
    Interface inter,
    std::string& log,
    CardContainer& dataBase,
    Entity& pOne,
    Entity& pTwo
) :
    log(log),
    dataBase(dataBase),
    interface(inter),
    playerOne(pOne),
    playerTwo(pTwo)
{
    Init();
}
void GameServer::Init()
{
    log.clear();
    log += std::string(intro);
    log += std::string(version);
}
void GameServer::RunServer(int opcode, int arg1, int arg2)
{
}
//if it returns > 0 (1 for now) then success, if 0 is returned then exit, if <0 is returned => error
int GameServer::RunConsole()
{
    std::string operation;
    std::string arg1;
    std::string arg2;

    GetInput(operation, arg1, arg2);

    int opcode = 0;
    int iarg1 = 0;
    int iarg2 = 0;

    GetCommandConsole(operation.c_str(), arg1.c_str(), arg2.c_str(), opcode, iarg1, iarg2);

    return RunCommand(opcode, iarg1, iarg2);
}
bool GameServer::GetInput(std::string &operation, std::string &arg1, std::string &arg2)
{
    system("CLS");

    std::cout << log.c_str();
    std::cout << "$ ";

    char command[30];
    std::cin.getline(command, 29);

    log += std::string("$ ");

    char* p = strtok(command, " ");
    for (int i = 0; i < 3; i++)
    {
        log += std::string(p);
        switch (i)
        {
        case 0:operation = p; break;
        case 1:arg1 = p; break;
        case 2:arg2 = p; break;
        default:break;
        }

        log += std::string(" ");

        p = strtok(NULL, " ");
    }

    log += std::string("\n");

    return true;
}
int GameServer::RunCommand(int opcode, int iarg1, int iarg2)
{
    switch (opcode)
    {
    case EXIT:
        return EXIT;
    case UNRECOGNIZED:
        log += "Command not found, use --help for a list of commands.\n";
        return UNRECOGNIZED;
    case SHOW:
        Show(iarg1);
        return true;

    }

    return false;
}
void GameServer::GetCommandConsole(const char* operation, const char* arg1, const char* arg2, int &opcode, int &iarg1, int &iarg2)
{
    if (CMP(operation, OP_EXIT))
    {
        opcode = EXIT;
        return;
    }
    if (CMP(operation, OP_SHOW))
    {
        if (CMP(arg1, "database"))
        {
            opcode = SHOW;
            iarg1 = 0;

            return;
        }
        if (CMP(arg1, "enemy") || (CMP(arg1, "player") && CMP(arg2, "2")))
        {
            opcode = SHOW;
            iarg1 = 2;

            return;
        }
        if (CMP(arg1, "") || CMP(arg1, "player") || (CMP(arg1, "player") && CMP(arg2, "1")))
        {
            opcode = SHOW;
            iarg1 = 1;

            return;
        }
    }

    //command not found
    opcode = UNRECOGNIZED;
    return;
}
void GameServer::Show(int iarg1)
{
    switch (iarg1)
    {
    case 0:
        log += std::string("Showing database:\n");

        for (auto child : dataBase.cards)
        {
            log += std::string("\t");
            log += std::string(child.name);
            log += std::string("\n");
        }

        return;
    case 1:
        log += std::string("Showing player hand:\n");

        for (auto child : playerOne.hand.cards)
        {
            log += std::string("\t");
            log += std::string(child.name);
            log += std::string("\n");
        }

        return;
    case 2:
        log += std::string("Showing enemy hand:\n");

        for (auto child : playerTwo.hand.cards)
        {
            log += std::string("\t");
            log += std::string(child.name);
            log += std::string("\n");
        }

        return;
    }

    return;
}