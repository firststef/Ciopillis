#pragma once
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerEvent.h"
#include "KeyboardInputComponent.h"

class ArenaSystem : public ISystem
{
    void OnInit(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        auto fadedBackground(pool->AddEntity());
        fadedBackground->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT });
        fadedBackground->Add<SpriteComponent>(std::string("FadedBackground"), Texture2D(), Fade(BLACK, 0.6f));
        arena.generatedEntities.push_back(fadedBackground);

        arena.player = pool->AddEntity();
        arena.player->Add<TransformComponent>(Rectangle{ 500,500,200,200 });
        arena.player->Add<SpriteComponent>(std::string("Fighter"), Texture2D(), Color(ORANGE));
        arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
        arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE,  500,500 , 200, 200, 1);
        auto& playerBody = arena.player->Get<PhysicsComponent>().body;
        playerBody->staticFriction = 1.0f;
        playerBody->freezeOrient = true;
        arena.generatedEntities.push_back(arena.player);

        arena.enemy = pool->AddEntity();
        arena.enemy->Add<TransformComponent>(Rectangle{ 200,200,200,200 });
        arena.enemy->Add<SpriteComponent>(std::string("Enemy"), Texture2D(), Color(BLUE));
        arena.enemy->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, 200,200 , 200, 200, 1);
        auto& enemyBody = arena.enemy->Get<PhysicsComponent>().body;
        enemyBody->staticFriction = 1.0f;
        enemyBody->freezeOrient = true;
        arena.generatedEntities.push_back(arena.enemy);

        arena.state = ArenaGameComponent::RUNNING;
    }

    void OnRunning(EntityPtr e)
    {

    }

    void OnEnd(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        arena.generatedEntities.clear();
    }

public:
    ArenaSystem() : ISystem(std::string("ArenaSystem")) {}

    void Initialize() override
    {
    }

    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
        {
            auto arena = e->Get<ArenaGameComponent>();

            switch (arena.state) {
            case ArenaGameComponent::INIT:
                OnInit(e);
            case ArenaGameComponent::RUNNING:
                OnRunning(e);
            case ArenaGameComponent::ENDED:
                OnEnd(e);
            default:
                break;
            }
        }
    }

    void Receive(const ArenaPlayerEvent& event)
    {
        if (event.action == ArenaPlayerEvent::ATTACK_X)
        {
            
        }
        else if (event.action == ArenaPlayerEvent::ATTACK_Y)
        {
            
        }


        if (event.action == ArenaPlayerEvent::MOVE) 
        {
            for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
            {
                auto& arena = e->Get<ArenaGameComponent>();

                auto& comp = arena.player->Get<PhysicsComponent>();
                comp.body->velocity = event.axes;
            }
        }
    }
};
