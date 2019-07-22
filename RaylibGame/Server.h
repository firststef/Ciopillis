#pragma once
#include "Classes.h"

#define CMP(x,y) (strcmp(x,y) == 0)

#define OP_SHOW "show"
#define OP_EXIT "exit"

class Entity
{
public:
    Types::SString                                 name;

    int                                     points = 0;

    CardContainer                           hand;
    CardContainer                           draw;
    CardContainer                           discard;

    Entity(const Types::SString &name) :
        name(name),
        hand(Types::SString(name) + Types::SString(" hand"), { 0,0,0,0 }),
        draw(Types::SString(name) + Types::SString(" draw"), { 0,0,0,0 }),
        discard(Types::SString(name) + Types::SString(" discard"), { 0,0,0,0 })
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

    Types::SString&                                log;

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
        Types::SString& log,
        CardContainer& dataBase,
        Entity& pOne,
        Entity& pTwo
    );

    //Class Functions
    void                                    Init();
    int                                     RunConsole();
    void                                    RunServer(int opcode, int arg1, int arg2);//the return type is to be decided
    bool                                    GetInput(Types::SString &operation, Types::SString &arg1, Types::SString &arg2);
    void                                    GetCommandConsole(const char* operation, const char* arg1, const char*arg2, int &opcode, int &iarg1, int &iarg2);
    int                                     RunCommand(int opcode, int iarg1, int iarg2);

    //Game Functions
    void                                    Show(int iarg1);
};