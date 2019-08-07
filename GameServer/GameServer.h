#pragma once
#include <string>
#include "Player.h"

#define CMP(x,y) (strcmp(x,y) == 0)

#define OP_SHOW "show"
#define OP_EXIT "exit"

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

    std::string&                            log;

    CardContainer&                          dataBase;
    Player&                                 playerOne;
    Player&                                 playerTwo;

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
        std::string& log,
        CardContainer& dataBase,
        Player& pOne,
        Player& pTwo
    );

    //Class Functions
    void                                    Init();
    int                                     RunConsole();
    void                                    RunServer(int opcode, int arg1, int arg2);//the return type is to be decided
    bool                                    GetInput(std::string &operation, std::string &arg1, std::string &arg2);
    void                                    GetCommandConsole(const char* operation, const char* arg1, const char*arg2, int &opcode, int &iarg1, int &iarg2);
    int                                     RunCommand(int opcode, int iarg1, int iarg2);

    //Game Functions
    void                                    Show(int iarg1);
};