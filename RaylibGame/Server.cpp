#include "Server.h"

GameServer::GameServer(
    Interface inter,
    Types::SString& log,
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
    log.Clear();
    log += Types::SString(intro);
    log += Types::SString(version);
}
void GameServer::RunServer(int opcode, int arg1, int arg2)
{
}
//if it returns > 0 (1 for now) then success, if 0 is returned then exit, if <0 is returned => error
int GameServer::RunConsole()
{
    Types::SString operation;
    Types::SString arg1;
    Types::SString arg2;

    GetInput(operation, arg1, arg2);

    int opcode = 0;
    int iarg1 = 0;
    int iarg2 = 0;

    GetCommandConsole(operation.GetText(), arg1.GetText(), arg2.GetText(), opcode, iarg1, iarg2);

    return RunCommand(opcode, iarg1, iarg2);
}
bool GameServer::GetInput(Types::SString &operation, Types::SString &arg1, Types::SString &arg2)
{
    system("CLS");

    cout << log.GetText();
    cout << "$ ";

    char command[30];
    cin.getline(command, 29);

    log += Types::SString("$ ");

    char* p = strtok(command, " ");
    for (int i = 0; i < 3; i++)
    {
        log += Types::SString(p);
        switch (i)
        {
        case 0:operation = p; break;
        case 1:arg1 = p; break;
        case 2:arg2 = p; break;
        default:break;
        }

        log += Types::SString(" ");

        p = strtok(NULL, " ");
    }

    log += Types::SString("\n");

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
        log += Types::SString("Showing database:\n");

        for (auto child : dataBase.children)
        {
            log += Types::SString("\t");
            log += Types::SString(child->name);
            log += Types::SString("\n");
        }

        return;
    case 1:
        log += Types::SString("Showing player hand:\n");

        for (auto child : playerOne.hand.children)
        {
            log += Types::SString("\t");
            log += Types::SString(child->name);
            log += Types::SString("\n");
        }

        return;
    case 2:
        log += Types::SString("Showing enemy hand:\n");

        for (auto child : playerTwo.hand.children)
        {
            log += Types::SString("\t");
            log += Types::SString(child->name);
            log += Types::SString("\n");
        }

        return;
    }

    return;
}