#pragma once
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerEvent.h"
#include "KeyboardInputComponent.h"
#include "Constants.h"

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
        arena.player->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load("../sprites/basesprite.PNG"), Color(ORANGE), Rectangle{ 0, 0, 29, 24});
        arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Y);
        arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE,  500,500 , 200, 200, 1);
        auto& playerBody = arena.player->Get<PhysicsComponent>().body;
        playerBody->staticFriction = 1.0f;
        playerBody->freezeOrient = true;
        arena.generatedEntities.push_back(arena.player);

        arena.enemy = pool->AddEntity();
        arena.enemy->Add<TransformComponent>(Rectangle{ 200,200,200,200 });
        arena.enemy->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load("../sprites/basesprite.PNG"), Color(BLUE), Rectangle{ 0, 0, 29, 24 });
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
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
        {
            auto& arena = e->Get<ArenaGameComponent>();

            if (event.action == ArenaPlayerEvent::ATTACK_X)
            {
                eventManager->Notify<AnimationEvent>(
                    arena.player,
                    std::string("Attack") + (event.axes.x > 0 ? std::string("Right") : std::string("Left")),
                    AnimationEvent::SOLVE_THIS_FIRST,
                    AnimationEvent::RESTART_SAME,
                    textureManager->Load("../sprites/basesprite.PNG"),
                    Rectangle{ (float)SPRITE_WIDTH * 6,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                    event.axes.x == 0 ? arena.lastAxesPlayer.x > 0 : event.axes.x > 0,
                    event.axes.x > 0,
                    3,
                    1,
                    100
                    );
            }
            else if (event.action == ArenaPlayerEvent::ATTACK_Y)
            {

            }

            if (event.action == ArenaPlayerEvent::MOVE)
            {
                auto& comp = arena.player->Get<PhysicsComponent>();
                comp.body->velocity = { event.axes.x * VELOCITY , event.axes.y * VELOCITY };

                if (event.axes == Vector2 {0, 0}) {
                    eventManager->Notify<AnimationEvent>(
                        arena.player,
                        std::string("Idle"),
                        AnimationEvent::OVERRIDE_OTHERS,
                        AnimationEvent::CONTINUE_IF_EXISTENT_SAME,
                        textureManager->Load("../sprites/basesprite.PNG"),
                        Rectangle{ 0,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                        arena.lastAxesPlayer.x > 0,
                        false,
                        1,
                        0,
                        -1
                        );
                }
                else {
                    eventManager->Notify<AnimationEvent>(
                        arena.player,
                        std::string("Move") + ((event.axes.x == 0 ? arena.lastAxesPlayer.x > 0 : event.axes.x > 0) ? std::string("Right") : std::string("Left")),
                        AnimationEvent::OVERRIDE_OTHERS,
                        AnimationEvent::CONTINUE_IF_EXISTENT_SAME,
                        textureManager->Load("../sprites/basesprite.PNG"),
                        Rectangle{ (float)SPRITE_WIDTH,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                        event.axes.x == 0 ? arena.lastAxesPlayer.x > 0 : event.axes.x > 0,
                        event.axes.x > 0,
                        4,
                        0,
                        135
                        );
                }

            }

            arena.lastAxesPlayer = { event.axes.x != 0 ? event.axes.x : arena.lastAxesPlayer.x, event.axes.y != 0 ? event.axes.y : arena.lastAxesPlayer.y };
        }
    }
};
