#pragma once
#include "Classes.h"
#include "Types.h"

#define CMP(x,y) (strcmp(x,y) == 0)

#define OP_SHOW "show"
#define OP_EXIT "exit"

class Entity
{
public:
    SString                                 name;

    int                                     points = 0;

    CardContainer                           hand;
    CardContainer                           draw;
    CardContainer                           discard;

    Entity(const SString &name) :
        name(name),
        hand(SString(name) + SString(" hand"), { 0,0,0,0 }),
        draw(SString(name) + SString(" draw"), { 0,0,0,0 }),
        discard(SString(name) + SString(" discard"), { 0,0,0,0 })
    {}
};

enum ServerOpCodes
{
    UNRECOGNIZED = -1,
    EXIT,
    SHOW
};

class GameServer
{
public:
    const char*                             intro = "<== Card Game Server for Ciopillis ==> \n";
    const char*                             version = "=== Version 0.1 ===\n";

    SString&                                log;

    CardContainer&                          dataBase;
    Entity&                                 playerOne;
    Entity&                                 playerTwo;

    enum Interface
    {
        SERVER,
        CONSOLE
    }                                       interface = SERVER;

    enum Turn
    {
        PLAYER_ONE,
        PLAYER_TWO
    }                                       currentTurn = PLAYER_ONE;

    enum GameType
    {
        SINGLEPLAYER,
        MULTIPLAYER
    }                                       gameType = SINGLEPLAYER;

    GameServer(
        Interface inter,
        SString& log,
        CardContainer& dataBase,
        Entity& pOne,
        Entity& pTwo
    );

    //Class Functions
    void                                    Init();
    int                                     RunConsole();
    void                                    RunServer(int opcode, int arg1, int arg2);//the return type is to be decided
    bool                                    GetInput(SString &operation, SString &arg1, SString &arg2);
    void                                    GetCommandConsole(const char* operation, const char* arg1, const char*arg2, int &opcode, int &iarg1, int &iarg2);
    int                                     RunCommand(int opcode, int iarg1, int iarg2);

    //Game Functions
    void                                    Show(int iarg1);
};
GameServer::GameServer(
    Interface inter, 
    SString& log,
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
    log += SString(intro);
    log += SString(version);
}
void GameServer::RunServer(int opcode, int arg1, int arg2)
{
}
//if it returns > 0 (1 for now) then success, if 0 is returned then exit, if <0 is returned => error 
int GameServer::RunConsole()
{
    SString operation;
    SString arg1;
    SString arg2;

    GetInput(operation, arg1, arg2);

    int opcode = 0;
    int iarg1 = 0;
    int iarg2 = 0;

    GetCommandConsole(operation.GetText(), arg1.GetText(), arg2.GetText(), opcode, iarg1, iarg2);

    return RunCommand(opcode, iarg1, iarg2);
}
bool GameServer::GetInput(SString &operation, SString &arg1, SString &arg2)
{
    system("CLS");

    cout << log;
    cout << "$ ";

    char command[30];
    cin.getline(command, 29);

    log += SString("$ ");

    char* p = strtok(command, " ");
    for (int i = 0; i < 2; i++)
    {
        log += SString(p);
        switch (i)
        {
        case 0:operation = p; break;
        case 1:arg1 = p; break;
        case 2:arg2 = p; break;
        default:break;
        }

        log += SString(" ");

        p = strtok(NULL, " ");
    }

    log += SString("\n");

    return true;
}
int GameServer::RunCommand(int opcode, int iarg1, int iarg2)
{
    switch(opcode)
    {
    case EXIT:
        return EXIT;
    case UNRECOGNIZED:
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
    switch(iarg1)
    {
    case 0:
        log += SString("Showing database:\n");

        for (auto child : dataBase.children)
        {
            log += SString("\t");
            log += SString(child->name);
            log += SString("\n");
        }

        return;
    case 1:
        log += SString("Showing player hand:\n");

        for (auto child : playerOne.hand.children)
        {
            log += SString("\t");
            log += SString(child->name);
            log += SString("\n");
        }

        return;
    case 2:
        log += SString("Showing enemy hand:\n");

        for (auto child : playerTwo.hand.children)
        {
            log += SString("\t");
            log += SString(child->name);
            log += SString("\n");
        }

        return;
    }

    return;
}