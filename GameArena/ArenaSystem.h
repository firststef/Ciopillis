#pragma once
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerEvent.h"
#include "Constants.h"

class ArenaSystem : public ISystem
{
    void OnInit(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        //Background Setup
        auto fadedBackground(pool->AddEntity());
        fadedBackground->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT });
        fadedBackground->Add<SpriteComponent>(std::string("FadedBackground"), Texture2D(), Fade(BLACK, 0.6f));
        arena.generatedEntities.push_back(fadedBackground);

        //Player Setup
        arena.player = pool->AddEntity();

        Rectangle player_rec{ 1000,500,200,200 };

        auto& transform = arena.player->Add<TransformComponent>(player_rec);
        arena.player->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load("D:\\GameDev\\Ciopillis\\sprites\\basesprite.png"), Color(ORANGE), Rectangle{ 0, 0, 29, 24});
        arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z);
        
        auto& playerBody = arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, player_rec.x, player_rec.y, 100, player_rec.height, 1).body;
        playerBody->staticFriction = 0.2f;
        playerBody->dynamicFriction = 0.2f;
        playerBody->freezeOrient = false;

        //Player Animation
        std::shared_ptr<AnimationNode> idle = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("idle"),
                textureManager->Load("D:\\GameDev\\Ciopillis\\sprites\\basesprite.png"),
                Rectangle{ 0,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                1,
                1,
                arena.playerOrientation,
                std::make_shared<bool>(false),
                0)
            );

        std::shared_ptr<AnimationNode> move = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("move"),
                textureManager->Load("D:\\GameDev\\Ciopillis\\sprites\\basesprite.png"),
                Rectangle{ (float)SPRITE_WIDTH,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                MOVE_ANIM_FRAMES,
                MOVE_ANIM_TIME / MOVE_ANIM_FRAMES,
                arena.playerOrientation,
                arena.playerOrientation,
                0
            )
            );

        std::shared_ptr<AnimationNode> attack_x = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("attack_x"),
                textureManager->Load("D:\\GameDev\\Ciopillis\\sprites\\basesprite.png"),
                Rectangle{ (float)SPRITE_WIDTH * 5,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                ATTACK_X_ANIM_FRAMES,
                ATTACK_X_ANIM_TIME / ATTACK_X_ANIM_FRAMES,
                arena.playerOrientation,
                std::make_shared<bool>(false),
                1)
            );

        std::shared_ptr<AnimationNode> attack_z = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("attack_y"),
                textureManager->Load("D:\\GameDev\\Ciopillis\\sprites\\basesprite.png"),
                Rectangle{ (float)SPRITE_WIDTH * 8,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
                ATTACK_Y_ANIM_FRAMES,
                ATTACK_Y_ANIM_TIME / ATTACK_Y_ANIM_FRAMES,
                arena.playerOrientation,
                std::make_shared<bool>(false),
                1)
            );

        //idle-move
        idle->Next(move, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
        }, &arena);
        move->Next(idle, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
        }, &arena);

        //idle, move - attack_x
        idle->Next(attack_x, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
        }, &arena);
        move->Next(attack_x, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
        }, &arena);

        attack_x->Next(idle, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            if (node.animationUnit->currentRepeat == 1)
            {
                arenaCtx.currentActionPlayer = ArenaGameComponent::IDLE;
                arenaCtx.blockPlayerInput = false;
            }

            return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
        }, &arena);
        attack_x->Next(move, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
        }, &arena);

        arena.player->Add<AnimationComponent>(AnimationGraph(idle));

        //idle, move, attack_x - attack_z
        idle->Next(attack_z, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
        }, &arena);
        move->Next(attack_z, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
        }, &arena);
        attack_x->Next(attack_z, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
        }, &arena);

        attack_z->Next(idle, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            if (node.animationUnit->currentRepeat == 1)
            {
                arenaCtx.currentActionPlayer = ArenaGameComponent::IDLE;
                arenaCtx.blockPlayerInput = false;
            }

            return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
        }, &arena);
        attack_z->Next(move, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
        }, &arena);
        attack_z->Next(attack_x, [](const AnimationNode& node, void* context) ->bool
        {
            auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

            return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
        }, &arena);

        arena.player->Add<AnimationComponent>(AnimationGraph(idle));

        //Player HitBox
        ShapeContainer player_idle_cont(
            [](void* context) -> Vector2 {
                auto& arena = *static_cast<ArenaGameComponent*>(context);
                auto pos = arena.player->Get<PhysicsComponent>().body->position;
                return Vector2{ pos.x , pos.y  };
            }, 
            &arena,
            [](void* context) -> float {
                auto& arena = *static_cast<ArenaGameComponent*>(context);
                return arena.player->Get<PhysicsComponent>().body->orient;
            }, 
            &arena, 
            true);

        Shape body("body", "player");
        body.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(BLUE, 0.4f));
        player_idle_cont.AddShape(body, Vector2{ -100,0 }, Vector2{ -1, 1 }, false);

        Shape fist("fist", "player");
        fist.SetRectangle(Rectangle{ 0,0,20,20 }, 0.0f, Fade(RED, 0.4f));
        player_idle_cont.AddShape(fist, Vector2{ 0,0 }, Vector2{ 1, 1 }, false);

        arena.player->Add<HitBoxComponent>(player_idle_cont);
        player_idle_cont.Update();

        arena.generatedEntities.push_back(arena.player);

        //Enemy Setup

        Rectangle enemy_rec{ 200,200,200,200 };

        arena.enemy = pool->AddEntity();
        arena.enemy->Add<TransformComponent>(enemy_rec);
        arena.enemy->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load("../sprites/basesprite.PNG"), Color(BLUE), Rectangle{ 0, 0, 29, 24 });
        
        auto& enemyBody = arena.enemy->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, enemy_rec.x, enemy_rec.y, 100, enemy_rec.height, 1).body;
        enemyBody->staticFriction = 0.2f;
        enemyBody->dynamicFriction = 0.2f;
        enemyBody->freezeOrient = true;

        //Enemy hitbox
        ShapeContainer enemy_idle_cont(
            [](void* context) -> Vector2 {
            auto& arena = *static_cast<ArenaGameComponent*>(context);
            auto pos = arena.enemy->Get<PhysicsComponent>().body->position;
            return Vector2{ pos.x - 50, pos.y - 100 };
        },
            &arena,
            [](void* context) -> float {
            auto& arena = *static_cast<ArenaGameComponent*>(context);
            return arena.enemy->Get<PhysicsComponent>().body->orient;
        },
            &arena,
            true);

        Shape enemy_body("body", "enemy");
        enemy_body.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(BLUE, 0.4f));
        enemy_idle_cont.AddShape(enemy_body, Vector2{ -100,0 }, Vector2{ -1, 1 }, false);

        /*Shape enemy_fist("fist", "enemy");
        enemy_fist.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(RED, 0.4f));
        enemy_idle_cont.AddShape(Vector2{ 0,0 }, enemy_fist, false);*/

        arena.enemy->Add<HitBoxComponent>(enemy_idle_cont);
        enemy_idle_cont.Update();

        arena.generatedEntities.push_back(arena.enemy);

        arena.state = ArenaGameComponent::RUNNING;
    }

    void OnRunning(EntityPtr e)
    {
        //Height sorting
        auto& arena = e->Get<ArenaGameComponent>();

        std::sort(arena.generatedEntities.begin(), arena.generatedEntities.end(), [](EntityPtr a, EntityPtr b)
        {
            if (a->Has(1 << GetComponentTypeID<TransformComponent>() | 1 << GetComponentTypeID<PhysicsComponent>())
                && b->Has(1 << GetComponentTypeID<TransformComponent>() | 1 << GetComponentTypeID<PhysicsComponent>()))
            {
                return (a->Get<TransformComponent>().position.y + a->Get<TransformComponent>().position.height)
                    > (b->Get<TransformComponent>().position.y + b->Get<TransformComponent>().position.height);
            }
            return false;
        });

        unsigned i = 0;
        unsigned size = arena.generatedEntities.size();
        for (auto& en : arena.generatedEntities)
        {
            if (en->Has(1 << GetComponentTypeID<TransformComponent>() | 1 << GetComponentTypeID<PhysicsComponent>()))
            {
                en->Get<TransformComponent>().zIndex = size - i;

                auto& velocity = en->Get<PhysicsComponent>().body->velocity;
                velocity = Vector2{ velocity.x * (1.0f - TOP_DOWN_VELOCITY_LOSS), velocity.y * (1.0f - TOP_DOWN_VELOCITY_LOSS) };

                if (velocity <= Vector2({ TOP_DOWN_VELOCITY_LOW_LIMIT, TOP_DOWN_VELOCITY_LOW_LIMIT }) 
                    && velocity >= Vector2({ -TOP_DOWN_VELOCITY_LOW_LIMIT, -TOP_DOWN_VELOCITY_LOW_LIMIT }))
                    velocity = Vector2{ 0,0 };
            }

            i++;

            //INFO: deci pot folosi obiecte fizice overlapped atata timp cat nu sunt activate
        }
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
        for (auto& e : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>()))
        {
            auto arena = e->Get<ArenaGameComponent>();

            switch (arena.state) {
            case ArenaGameComponent::INIT:
                OnInit(e);
                break;
            case ArenaGameComponent::RUNNING:
                OnRunning(e);
                break;
            case ArenaGameComponent::ENDED:
                OnEnd(e);
                break;
            default:
                break;
            }
        }
    }

    void Receive(const ArenaPlayerEvent& event)
    {
        //TODO: aici trebuie verificat daca jucatorul tine apasat pe hold, daca a trecut timpul de cooldown
        for (auto& e : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>()))
        {
            auto& arena = e->Get<ArenaGameComponent>();
            auto& comp = arena.player->Get<PhysicsComponent>();
            auto& box = arena.player->Get<HitBoxComponent>();

            arena.lastAxesPlayer = { event.axes.x != 0 ? event.axes.x : arena.lastAxesPlayer.x, event.axes.y != 0 ? event.axes.y : arena.lastAxesPlayer.y };
            *arena.playerOrientation = arena.lastAxesPlayer.x > 0;

            box.cont.Mirror(Vector2{ arena.lastAxesPlayer.x, 1 });
            box.cont.Update(true);

            if (arena.blockPlayerInput)
                continue;

            if (event.action == ArenaPlayerEvent::ATTACK_X)
            {
                arena.currentActionPlayer = ArenaGameComponent::ATTACK_X;
                arena.blockPlayerInput = true;
                comp.body->velocity = { 0,0 };
            }
            else if (event.action == ArenaPlayerEvent::ATTACK_Z)
            {
                arena.currentActionPlayer = ArenaGameComponent::ATTACK_Z;
                arena.blockPlayerInput = true;
                comp.body->velocity = { 0,0 };
            }
            else if (event.action == ArenaPlayerEvent::MOVE)
            {
                comp.body->velocity = { event.axes.x * VELOCITY , event.axes.y * VELOCITY };

                if (event.axes == Vector2 {0, 0}) {
                    arena.currentActionPlayer = ArenaGameComponent::IDLE;
                }
                else {
                    arena.currentActionPlayer = ArenaGameComponent::MOVE;
                }
            }
        }
    }

    void Receive(const HitBoxEvent& event)
    {
        auto test = 3;
    }
};
