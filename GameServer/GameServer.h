#pragma once
#include <string>
#include "Player.h"

enum ServerOpCodes
{
    UNRECOGNIZED = -1,
    EXIT,
    START,
    SHOW,
    ADD,
    DRAW,
    DISCARD,
    DELETE,
    END_TURN,
    PLAY_CARD,
    CONFRONT_CARDS,
    EVAL
};

class GameServer
{
public:
    const char*                             intro = "<== Card Game Server for Ciopillis ==> \n";
    const char*                             version = "=== Version 0.2 ===\n";

    std::string&                            log;

    CardContainer&                          dataBase;

    Player&                                 playerOne;
    Player&                                 playerTwo;

    enum Interface
    {
        SERVER,
        CONSOLE
    }                                       interface = SERVER;

    enum Identifiers
    {
        PLAYER_ONE = 1,
        PLAYER_TWO = 2,
        CONFRONT = 3
    }                                       currentTurn = PLAYER_ONE,
                                            previousTurn = CONFRONT;

    enum TurnAction
    {
        ATTACK = 0,
        DEFEND = 1,
    }                                       turnAction = ATTACK;

    enum GameState
    {
        RUNNING = 1,
        ENDED = 0
    }                                       gameState = RUNNING;

    enum GameType
    {
        SINGLEPLAYER,
        MULTIPLAYER
    }                                       gameType = SINGLEPLAYER;

    enum ServerErrors
    {
        GENERIC = -1,
        WRONG_TURN = -2,
        CONTAINER_EMPTY = -3,
        BAD_INDEX = -4,
        CANNOT_DRAW = -5,
        CANNOT_PLAY_CARD = -6,
        INVALID_CARD = -7,
        CARD_NOT_PLAYED =-8
    };

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
    int                                     RunServer(int opcode, int iarg1, int iarg2);//the return type is to be decided
    bool                                    GetInput(std::string &operation, std::string &arg1, std::string &arg2);
    void                                    GetCommandConsole(const char* operation, const char* arg1, const char*arg2, int &opcode, int &iarg1, int &iarg2);
    int                                     RunCommand(int opcode, int iarg1, int iarg2);

    //Game Functions
    void                                    Start();
    void                                    Show(int iarg1);
    void                                    ShowCard(const Card& card);
    void                                    Add(int iarg1, unsigned iarg2);
    int                                     Draw(int iarg1);
    int                                     Discard(int iarg1, unsigned iarg2);
    int                                     Delete(int iarg1, unsigned iarg2);
    int                                     EndTurn(int iarg1);
    int                                     Play(int iarg1, unsigned iarg2);
    int                                     Confront();
    int                                     Eval();
    
    int                                     vm_run_card_functionality(const Card& card);
    int                                     validate_play_card(const Card& card);
};