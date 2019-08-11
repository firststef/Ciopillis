#pragma once
#include "Components.h"
#include "System.h"
#include "GridContainerSystem.h"
#include "../GameServer/GameServer.h"
#include "../GameServer/CardGenerator.h"
class EventSystem : public ISystem
{
public:

    GameServer& server;

    EntityPtr dragParentOrigin;
    int indexInDragParentOrigin;

    EventSystem(GameServer& server) : server(server) {}

    void Initialize() override
    {
        server.RunServer(START, -1,-1);
    }

    void Execute() override
    {
    }

    void Receive(const MouseEvent& event)
    {
        if (event.type == MouseEvent::MOUSE_PRESS)
        {
            std::printf("Press\n");

            if (event.entity->Get<SpriteComponent>().name == std::string("Endturn Button"))
            {
                if (server.RunServer(END_TURN, 1, -1) > 0) {

                    const auto turn = server.RunServer(TURN, -1, -1);

                    if ( turn == GameServer::PLAYER_TWO)
                    {
                        eventManager->Notify<EnemyEvent>(EnemyEvent::ENEMY_TURN);
                    }
                    if ( turn == GameServer::CONFRONT)
                    {
                        server.RunServer(CONFRONT_CARDS, -1, -1);
                    }
                }
            }
        } 
        else if(event.type == MouseEvent::MOUSE_BEGIN_DRAG)
        {
            std::printf("Begin\n");

            if (event.entity->Has(1 << GetTypeID<GridContainerChildComponent>())) {
                auto& childC = event.entity->Get<GridContainerChildComponent>();

                dragParentOrigin = childC.parent; 
                indexInDragParentOrigin = childC.indexInParent;

                eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::REMOVE, event.entity, dragParentOrigin);
            }
        }
        else if (event.type == MouseEvent::MOUSE_CONTINUE_DRAG) 
        {
            std::printf("Continue\n");
        }
        else if (event.type == MouseEvent::MOUSE_END_DRAG) 
        {
            std::printf("End\n");

            const auto coord = event.entity->Get<TransformComponent>().position;
            Vector2 center = { coord.x + coord.width / 2, coord.y + coord.height / 2 };
;
            auto objects = pool->GetEntities(1 << GetTypeID<TransformComponent>());
            std::sort(objects.begin(), objects.end(), [](EntityPtr left, EntityPtr right)->bool
            {
                return left->Get<TransformComponent>().zIndex > right->Get<TransformComponent>().zIndex;//aici nu stiu ce compara 
            });

            EntityPtr highestParentObject;
            for (auto& zObj : objects)
            {
                if (zObj->Has(1 << GetTypeID<GridContainerComponent>()))
                {
                    const auto parentCoord = zObj->Get<TransformComponent>().position;

                    if (CheckCollisionPointRec(center, parentCoord))
                    {
                        highestParentObject = zObj;
                        break;
                    }
                }
            }

            if (highestParentObject != nullptr) {

                auto sourceObjectSprite = dragParentOrigin->Get<SpriteComponent>();
                auto destObjectSprite = highestParentObject->Get<SpriteComponent>();

                if (sourceObjectSprite.name == std::string("Draw") && destObjectSprite.name == std::string("Hand"))
                {
                    const auto idx = server.RunServer(DRAW, 1, -1);
                    if (idx > 0) {
                        auto newCard = pool->AddEntity();
                        newCard->Add<TransformComponent>();
                        newCard->Get<TransformComponent>().position = { -500,-500, CARD_WIDTH, CARD_HEIGHT };
                        newCard->Add<SpriteComponent>(std::string("Card"), Color(PINK));
                        newCard->Add<MouseInputComponent>(std::bitset<32>((1 << MouseInputComponent::DRAG) | (1 << MouseInputComponent::PRESS) | (1 << MouseInputComponent::SELECT)));
                        newCard->Add<CardComponent>(server.dataBase.cards[idx - 1]);

                        eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, newCard, highestParentObject);
                        eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, dragParentOrigin);

                        return;
                    }
                }
                if (sourceObjectSprite.name == std::string("Hand") && destObjectSprite.name == std::string("Discard"))
                {
                    if (server.RunServer(DISCARD, 1, indexInDragParentOrigin) > 0) {
                        eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, highestParentObject);
                        return;
                    }
                }
                if (sourceObjectSprite.name == std::string("Hand") && destObjectSprite.name == std::string("Play Zone"))
                {
                    if (server.RunServer(PLAY_CARD, 1, indexInDragParentOrigin) > 0) {
                        eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, highestParentObject);
                        return;
                    }
                }
            }
            
            eventManager->Notify<GridAddRemoveEvent>(GridAddRemoveEvent::ADD, event.entity, dragParentOrigin);
        }
    }
};
