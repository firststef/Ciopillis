#pragma once
#include "GameServer.h"
#include "CardGameEnemyEvent.h"

class CardGameEnemySystem : public ISystem
{
public:

    GameServer& server;

    EntityPtr& enemyHand;
    EntityPtr& enemyDraw;
    EntityPtr& enemyDiscard;
    
    EntityPtr& playZone;

    CardGameEnemySystem(GameServer& server, EntityPtr& enemyHand, EntityPtr& enemyDraw, EntityPtr& enemyDiscard, EntityPtr& playZone) 
    : ISystem(std::string("CardGameEnemySystem")), server(server), enemyHand(enemyHand), enemyDraw(enemyDraw), enemyDiscard(enemyDiscard), playZone(playZone)
    {
    }

    void Initialize() override
    {
		if (pool == nullptr)
			throw MissingDependencyException("Entity Pool");
		if (textureManager == nullptr)
			throw MissingDependencyException("Texture manager");
		if (eventManager == nullptr)
			throw MissingDependencyException("Event manager");
    }

    void Execute() override
    {
    }

    void Receive(const CardGameEnemyEvent& enemy)
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
                newCard->Add<SpriteComponent>(server.dataBase.cards[idx - 1].name, textureManager->Load((std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "cards" / "backface.png").string()), Color(WHITE));
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

            if (cardIdx == server.playerTwo.hand->cards.size())
            {
                server.log += std::string("No card available for play\n");
                return;
            }
        }

        for (auto card : enemyHand->Get<GridContainerComponent>().items)
        {
            auto& cardComp = card->Get<CardComponent>();
            auto& spriteComp = card->Get<SpriteComponent>();
            if (cardComp.card.id == server.playerTwo.hand->cards[server.playerTwo.selectedCardIndex].id)
            {
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::REMOVE, card, enemyHand);
                textureManager->Unload(spriteComp.texture);
                spriteComp.texture = textureManager->Load(server.dataBase.cards[cardComp.card.id - 1].path.c_str());
                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, card, playZone);
                break;
            }
        }
    }
};