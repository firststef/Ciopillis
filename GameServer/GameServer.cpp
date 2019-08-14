#include "GameServer.h"
#include <iostream>
#include <algorithm>
#include <locale>
#include <random>

#define CMP(x,y) (strcmp(x,y) == 0)

#define OP_START "start"
#define OP_SHOW "show"
#define OP_EXIT "exit"
#define OP_ADD "add"
#define OP_DRAW "draw"
#define OP_DISCARD "discard"
#define OP_DELETE "delete"
#define OP_END_TURN "endturn"
#define OP_PLAY_CARD "play"
#define OP_EVAL "eval"
#define OP_CONFRONT "confront"
#define OP_TURN "turn"

bool is_number(const std::string & s)
{
    for (char c : s) if (!isdigit(c)) return false;
    return true;
}

GameServer::GameServer(Interface inter, std::string & log, CardContainer & dataBase, Player & pOne, Player & pTwo)
    : interface(inter), log(log), dataBase(dataBase), playerOne(pOne), playerTwo(pTwo)
{
    Init();
}
void GameServer::Init()
{
    log.clear();
    log += std::string(intro);
    log += std::string(version);
}
int GameServer::RunServer(int opcode, int iarg1, int iarg2)
{
    return RunCommand(opcode, iarg1, iarg2);
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
        if (p == nullptr)
            break;

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
    case START:
        Start();
        return true;
    case EXIT:
        return EXIT;
    case SHOW:
        Show(iarg1);
        return true;
    case ADD:
        Add(iarg1, iarg2);
        return true;
    case DRAW:
        return Draw(iarg1);
    case DISCARD:
        return Discard(iarg1, iarg2);
    case DELETE:
        return Delete(iarg1, iarg2);
    case END_TURN:
        return EndTurn(iarg1);
    case PLAY_CARD:
        return Play(iarg1,iarg2);
    case EVAL:
        return Eval();
    case CONFRONT_CARDS:
        return Confront();
    case TURN:
        return Turn();
    default:
    case UNRECOGNIZED:
        log += "Command not found, use --help for a list of commands.\n";
        return GENERIC;
    }
}
void GameServer::GetCommandConsole(const char* operation, const char* arg1, const char* arg2, int &opcode, int &iarg1, int &iarg2)
{
    opcode = UNRECOGNIZED;

    if (CMP(operation, OP_EXIT))
    {
        opcode = EXIT;
    }
    else if (CMP(operation, OP_START))
    {
        opcode = START;
    }
    else if (CMP(operation, OP_SHOW))
    {
        if (CMP(arg1, "database"))
        {
            opcode = SHOW;
            iarg1 = 0;
        }
        else if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            opcode = SHOW;
            iarg1 = PLAYER_TWO;
        }
        else if (CMP(arg1, "") || CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            opcode = SHOW;
            iarg1 = PLAYER_ONE;
        }
    }
    else if (CMP(operation, OP_ADD))
    {
        if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            if(is_number(std::string(arg2)))
            {
                opcode = ADD;
                iarg1 = PLAYER_TWO;
                iarg2 = atoi(arg2);
            }
        }
        else if (CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            if (is_number(std::string(arg2)))
            {
                opcode = ADD;
                iarg1 = PLAYER_ONE;
                iarg2 = atoi(arg2);
            }
        }
    }
    else if (CMP(operation, OP_DRAW))
    {
        if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            opcode = DRAW;
            iarg1 = PLAYER_TWO;
        }
        else if (CMP(arg1, "") || CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            opcode = DRAW;
            iarg1 = PLAYER_ONE;
        }
    }
    else if (CMP(operation, OP_DISCARD))
    {
        if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            if (is_number(std::string(arg2)))
            {
                opcode = DISCARD;
                iarg1 = PLAYER_TWO;
                iarg2 = atoi(arg2);
            }
        }
        else if (CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            if (is_number(std::string(arg2)))
            {
                opcode = DISCARD;
                iarg1 = PLAYER_ONE;
                iarg2 = atoi(arg2);
            }
        }
    }
    else if (CMP(operation, OP_DELETE))//ar merge o refactorizare de ordine a operatiilor pentru a scapa de cod duplicat
    {
        if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            if (is_number(std::string(arg2)))
            {
                opcode = DISCARD;
                iarg1 = PLAYER_TWO;
                iarg2 = atoi(arg2);
            }
        }
        else if (CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            if (is_number(std::string(arg2)))
            {
                opcode = DISCARD;
                iarg1 = PLAYER_ONE;
                iarg2 = atoi(arg2);
            }
        }
    }
    else if (CMP(operation, OP_END_TURN))
    {
        if (CMP(arg1, "enemy") || CMP(arg1, "player2"))
        {
            opcode = END_TURN;
            iarg1 = PLAYER_TWO;
        }
        else if (CMP(arg1, "") || CMP(arg1, "player") || CMP(arg1, "player1"))
        {
            opcode = END_TURN;
            iarg1 = PLAYER_ONE;
        }
    }
    else if (CMP(operation, OP_PLAY_CARD))
    {
        if (is_number(std::string(arg1)) && is_number(std::string(arg2)))
        {
            opcode = PLAY_CARD;
            iarg1 = atoi(arg1);
            iarg2 = atoi(arg2);
        }
    }
    else if (CMP(operation, OP_EVAL))
    {
        opcode = EVAL;
    }
    else if (CMP(operation, OP_CONFRONT))
    {
        opcode = CONFRONT_CARDS;
    }
    else if (CMP(operation, OP_TURN))
    {
        opcode = TURN;
    }
}

void GameServer::Start()
{
    playerOne.hand.cards.clear();
    playerTwo.hand.cards.clear();
    playerOne.discard.cards.clear();
    playerTwo.discard.cards.clear();

    playerOne.draw.cards = playerOne.base.cards;
    std::shuffle(playerOne.draw.cards.begin(), playerOne.draw.cards.end(), std::default_random_engine(time(NULL)));

    playerTwo.draw.cards = playerTwo.base.cards;
    std::shuffle(playerTwo.draw.cards.begin(), playerTwo.draw.cards.end(), std::default_random_engine(time(NULL) + 1000));

    log += std::string("Game started\n");
}
void GameServer::Show(int iarg1)
{
    int idx = 0;

    switch (iarg1)
    {
    case 0:
        log += std::string("Showing database:\n");

        for (auto& child : dataBase.cards)
        {
            log += std::string("\t");
            log += std::string(std::to_string(idx));
            log += std::string(std::string(":"));
            ShowCard(child);
            log += std::string("\n");
            ++idx;
        }

        return;
    case PLAYER_ONE:
        log += std::string("Showing player hand:\n");

        for (auto& child : playerOne.hand.cards)
        {
            log += std::string("\t");
            log += std::string(std::to_string(idx));
            log += std::string(std::string(":"));
            ShowCard(child);
            log += std::string("\n");
            ++idx;
        }

        return;
    case PLAYER_TWO:
        log += std::string("Showing enemy hand:\n");

        for (auto& child : playerTwo.hand.cards)
        {
            log += std::string("\t");
            log += std::string(std::to_string(idx));
            log += std::string(std::string(":"));
            ShowCard(child);
            log += std::string("\n");
            ++idx;
        }
        return;
    default: ;
    }
}
void GameServer::ShowCard(const Card& card)
{
    log += std::string(card.name);
    log += std::string("\n\t\t");
    log += std::string("Type:");
    log += std::string(std::to_string(card.type));
}
void GameServer::Add(int iarg1, unsigned iarg2)
{
    if (iarg2 < 0 || iarg2 >= dataBase.cards.size())
    {
        log += std::string("Incorrect card index\n");
        return;
    }

    if (iarg1 == PLAYER_ONE) {
        playerOne.hand.cards.push_back(dataBase.cards[iarg2]);
        log += std::string("Added card to player1 hand\n");
    }
    else if (iarg1 == PLAYER_TWO) {
        playerTwo.hand.cards.push_back(dataBase.cards[iarg2]);
        log += std::string("Added card to player2 hand\n");
    }
}
int GameServer::Draw(int iarg1)
{
    if (currentTurn != iarg1)
    {
        log += "Wrong turn operation\n";
        return WRONG_TURN;
    }

    if (iarg1 == PLAYER_ONE) {
        if (playerOne.hand.cards.size() == playerOne.maxCards)
        {
            log += "Cannot draw card\n";
            return CANNOT_DRAW;
        }

        if (!playerOne.draw.cards.empty())
        {
            playerOne.hand.cards.push_back(playerOne.draw.cards.back());
            playerOne.draw.cards.pop_back();
            log += std::string("Added card to player1 hand\n");
            return playerOne.hand.cards.back().id + 1;
        }
    }
    else if (iarg1 == PLAYER_TWO) {
        if (playerTwo.hand.cards.size() == playerTwo.maxCards)
        {
            log += "Cannot draw card\n";
            return CANNOT_DRAW;
        }

        if (!playerTwo.draw.cards.empty())
        {
            playerTwo.hand.cards.push_back(playerTwo.draw.cards.back());
            playerTwo.draw.cards.pop_back();
            log += std::string("Added card to player2 hand\n");
            return playerTwo.hand.cards.back().id + 1;
        }
    }
    log += std::string("No cards in draw\n");
    return CONTAINER_EMPTY;
}
int GameServer::Discard(int iarg1, unsigned iarg2)
{
    if (currentTurn != iarg1)
    {
        log += "Wrong turn operation\n";
        return WRONG_TURN;
    }

    if (iarg1 == PLAYER_ONE) {
        if (!playerOne.hand.cards.empty())
        {
            if (iarg2 < playerOne.hand.cards.size())
            {
                playerOne.discard.cards.push_back(*(playerOne.hand.cards.begin() + iarg2));
                playerOne.hand.cards.erase(playerOne.hand.cards.begin() + iarg2);
                log += std::string("Removed card from player1 hand\n");
                return 1;
            }
            
            return BAD_INDEX;
        }
    } else if (iarg1 == PLAYER_TWO) {
        if (!playerTwo.hand.cards.empty())
        {
            if (iarg2 < playerTwo.hand.cards.size())
            {
                playerTwo.discard.cards.push_back(*(playerTwo.hand.cards.begin() + iarg2));
                playerTwo.hand.cards.erase(playerTwo.hand.cards.begin() + iarg2);
                log += std::string("Removed card from player2 hand\n");
                return 1;
            }

            return BAD_INDEX;
        }
    }

    log += std::string("No cards in hand\n");
    return CONTAINER_EMPTY;
}
int GameServer::Delete(int iarg1, unsigned iarg2)
{
    if (iarg1 == PLAYER_ONE) {
        if (!playerOne.hand.cards.empty())
        {
            if (iarg2 < playerOne.hand.cards.size())
            {
                playerOne.hand.cards.erase(playerOne.hand.cards.begin() + iarg2);
                log += std::string("Removed card from player1 hand\n");
                return 1;
            }

            return BAD_INDEX;
        }
    } else if (iarg1 == PLAYER_TWO) {
        if (!playerTwo.hand.cards.empty())
        {
            if (iarg2 < playerTwo.hand.cards.size())
            {
                playerTwo.hand.cards.erase(playerTwo.hand.cards.begin() + iarg2);
                log += std::string("Removed card from player2 hand\n");
                return 1;
            }

            return BAD_INDEX;
        }
    }

    log += std::string("No cards in hand\n");
    return CONTAINER_EMPTY;
}//aici ar merge sa schimb functiile sa primeasca direct un obiect de tip player ca sa nu duplic cod
int GameServer::EndTurn(int iarg1)
{
    if (iarg1 == PLAYER_ONE)
    {
        if (currentTurn != PLAYER_ONE)
        {
            log += std::string("Not player turn to be ended\n");
            return GENERIC;
        }

        if (!playerOne.cardPlayed)
        {
            log += std::string("You have not played a card this round\n");
            return CARD_NOT_PLAYED;
        }

        if (turnAction == DEFEND) {
            previousTurn = currentTurn;
            currentTurn = CONFRONT;
        }
        else
        {
            currentTurn = Identifiers(currentTurn % 2 + 1);
        }

        turnAction = TurnAction((turnAction + 1) % 2);

        log += std::string("Turn changed\n");
        return 1;
    }
    else if (iarg1 == PLAYER_TWO)
    {
        if (currentTurn != PLAYER_TWO)
        {
            log += std::string("Not player turn to be ended\n");
            return GENERIC;
        }

        if (!playerTwo.cardPlayed)
        {
            log += std::string("You have not played a card this round\n");
            return CARD_NOT_PLAYED;
        }

        if (turnAction == DEFEND) {
            previousTurn = currentTurn;
            currentTurn = CONFRONT;
        }
        else
        {
            currentTurn = Identifiers(currentTurn % 2 + 1);
        }

        turnAction = TurnAction((turnAction + 1) % 2);

        log += std::string("Turn changed\n");
        return 1;
    }

    return GENERIC;
}
int GameServer::Play(int iarg1, unsigned iarg2)
{
    if (currentTurn != iarg1)
    {
        log += "Wrong turn operation\n";
        return WRONG_TURN;
    }

    if (iarg1 == PLAYER_ONE) {
        if (!playerOne.hand.cards.empty())
        {
            if (iarg2 < playerOne.hand.cards.size())
            {
                if (playerOne.cardPlayed)
                {
                    log += std::string("Already played a card\n");
                    return CANNOT_PLAY_CARD;
                }
                if (validate_play_card(playerOne.hand.cards[iarg2]) == INVALID_CARD)
                    return CANNOT_PLAY_CARD;

                playerOne.cardPlayed = 1;
                log += std::string("Played card from player1 hand\n");

                playerOne.selectedCardIndex = iarg2;
                return 1;
            }
        }
        return BAD_INDEX;
    }
    else if (iarg1 == PLAYER_TWO) {
        if (!playerTwo.hand.cards.empty())
        {
            if (iarg2 < playerTwo.hand.cards.size())
            {
                if (playerTwo.cardPlayed)
                {
                    log += std::string("Already played a card\n");
                    return CANNOT_PLAY_CARD;
                }
                if (validate_play_card(playerTwo.hand.cards[iarg2]) == INVALID_CARD)
                    return CANNOT_PLAY_CARD;

                playerTwo.cardPlayed = 1;
                log += std::string("Played card from player2 hand\n");

                playerTwo.selectedCardIndex = iarg2;
                return 1;
            }
        }
        return BAD_INDEX;
    }

    return GENERIC;
}
int GameServer::Confront()
{
    if (currentTurn != CONFRONT || playerOne.selectedCardIndex == -1 || playerTwo.selectedCardIndex == -1)
    {
        log += std::string("Play cards not selected\n");
        return CARD_NOT_PLAYED;
    }

    log += std::string("Playing ");
    log += playerOne.hand.cards[playerOne.selectedCardIndex].name;
    log += std::string(" against ");
    log += playerTwo.hand.cards[playerTwo.selectedCardIndex].name;
    log += std::string("\n");
        
    int result;
    if (true)//aici se verifica care carte e mai bun
        result = vm_run_card_functionality(playerOne.hand.cards[playerOne.selectedCardIndex]);
    else
        result = vm_run_card_functionality(playerTwo.hand.cards[playerTwo.selectedCardIndex]);

    currentTurn = previousTurn;

    playerOne.selectedCardIndex = -1;
    playerTwo.selectedCardIndex = -1;
    playerOne.cardPlayed = false;
    playerTwo.cardPlayed = false;

    return result;
}
int GameServer::Eval()
{
    if (playerOne.points <= 0)
    {
        log += std::string("Player2 wins");//ar trebui scos automat numele
        return 2;
    }
    if (playerTwo.points <= 0)
    {
        log += std::string("Player2 wins");
        return 1;
    }
    return 0;
}
int GameServer::Turn()
{
    return currentTurn;
}

int GameServer::vm_run_card_functionality(const Card& card)
{
    return GENERIC;
}
int GameServer::validate_play_card(const Card& card)
{
    if (card.type == VERSATILE || card.type == turnAction)
    {
        return true;
    }

    log += std::string("Card invalid\n");
    return INVALID_CARD;

}
