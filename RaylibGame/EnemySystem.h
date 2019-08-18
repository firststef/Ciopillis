#pragma once
#include "../GameServer/GameServer.h"
#include "EnemyEvent.h"
#include "../ECSlib/External.h"

class EnemySystem : public ISystem
{
public:

    GameServer& server;

    EntityPtr& enemyHand;
    EntityPtr& enemyDraw;
    EntityPtr& enemyDiscard;
    
    EntityPtr& playZone;

    EnemySystem(GameServer& server, EntityPtr& enemyHand, EntityPtr& enemyDraw, EntityPtr& enemyDiscard, EntityPtr& playZone) 
    : server(server), enemyHand(enemyHand), enemyDraw(enemyDraw), enemyDiscard(enemyDiscard), playZone(playZone), ISystem(std::string("EnemySystem"))
    {
    }

    void Initialize() override
    {
    }

    void Execute() override
    {
    }

    void Receive(const EnemyEvent& enemy)
    {
        eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, std::string("MouseInputSystem"));

        PlayTurn();

        if (server.RunServer(END_TURN, 2, -1) > 0) {

            auto turn = server.RunServer(TURN, -1, -1);

            if (turn == GameServer::PLAYER_TWO)
            {
                PlayTurn();

                server.RunServer(END_TURN, 2, -1);
            }
            if (turn == GameServer::CONFRONT)
            {
                server.RunServer(CONFRONT_CARDS, -1, -1);
            }
        }

        eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, std::string("MouseInputSystem"));
    }

    void PlayTurn()
    {
        while (true)
        {
            //SleepFunc(100);
            const auto idx = server.RunServer(DRAW, 2, -1);
            if (idx > 0) {

                auto newCard = pool->AddEntity();
                newCard->Add<TransformComponent>();
                newCard->Get<TransformComponent>().position = { -500,-500, CARD_WIDTH, CARD_HEIGHT };
                newCard->Add<SpriteComponent>(server.dataBase.cards[idx - 1].name, textureManager->Load(server.dataBase.cards[idx - 1].path.c_str()), Color(WHITE));
                newCard->Add<MouseInputComponent>(std::bitset<32>(0));
                newCard->Add<CardComponent>(server.dataBase.cards[idx - 1]);

                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, newCard, enemyHand);

                //SleepFunc(100);
            }
            else {
                break;
            }
        }

        //SleepFunc(700);

        unsigned cardIdx = 0;
        while (true)
        {
            const int result = server.RunServer(PLAY_CARD, 2, cardIdx);
            if (result > 0)
                break;

            cardIdx++;

            if (cardIdx == server.playerTwo.hand.cards.size())
            {
                server.log += std::string("No card available for play\n");
                return;
            }
        }

        for (auto card : enemyHand->Get<GridContainerComponent>().items)
        {
            if (card->Get<CardComponent>().card.id == server.playerTwo.hand.cards[server.playerTwo.selectedCardIndex].id)
            {
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::REMOVE, card, enemyHand);
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, card, playZone);
                break;
            }
        }
    }
};