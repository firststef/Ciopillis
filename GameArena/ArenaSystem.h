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
        arena.player->Add<TransformComponent>(Rectangle{ 500,500,200,200 });
        arena.player->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load("../sprites/basesprite.PNG"), Color(ORANGE), Rectangle{ 0, 0, 29, 24});
        arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z);
        arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE,  200, 500 , 100, 200, 1);
        auto& playerBody = arena.player->Get<PhysicsComponent>().body;
        playerBody->staticFriction = 0.2f;
        playerBody->dynamicFriction = 0.2f;
        playerBody->freezeOrient = true;

        //Player Animation
        std::shared_ptr<AnimationNode> idle = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("idle"),
                textureManager->Load("../sprites/basesprite.PNG"),
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
                textureManager->Load("../sprites/basesprite.PNG"),
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
                textureManager->Load("../sprites/basesprite.PNG"),
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
                textureManager->Load("../sprites/basesprite.PNG"),
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
        /*ShapeContainer* container = new ShapeContainer(playerBody->position, playerBody->orient, true);
        Shape* shape = new Shape;
        shape->SetRectangle(Rectangle{ 0,0,100,100 }, 0.0f, BLUE);
        container->AddShape(Vector2{ 300,300 }, *shape, false);*/

        arena.generatedEntities.push_back(arena.player);

        //Enemy Setup
        arena.enemy = pool->AddEntity();
        arena.enemy->Add<TransformComponent>(Rectangle{ 200,200,200,200 });
        arena.enemy->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load("../sprites/basesprite.PNG"), Color(BLUE), Rectangle{ 0, 0, 29, 24 });
        arena.enemy->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, 200, 200 , 100, 200, 1);
        auto& enemyBody = arena.enemy->Get<PhysicsComponent>().body;
        enemyBody->staticFriction = 0.2f;
        enemyBody->dynamicFriction = 0.2f;
        enemyBody->freezeOrient = true;

        arena.generatedEntities.push_back(arena.enemy);

        arena.state = ArenaGameComponent::RUNNING;
    }

    void OnRunning(EntityPtr e)
    {
        //Height sorting
        auto& arena = e->Get<ArenaGameComponent>();

        std::sort(arena.generatedEntities.begin(), arena.generatedEntities.end(), [](EntityPtr a, EntityPtr b)
        {
            if (a->Has(1 << GetTypeID<TransformComponent>() | 1 << GetTypeID<PhysicsComponent>())
                && b->Has(1 << GetTypeID<TransformComponent>() | 1 << GetTypeID<PhysicsComponent>()))
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
            if (en->Has(1 << GetTypeID<TransformComponent>() | 1 << GetTypeID<PhysicsComponent>()))
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
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
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
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
        {
            auto& arena = e->Get<ArenaGameComponent>();
            auto& comp = arena.player->Get<PhysicsComponent>();

            arena.lastAxesPlayer = { event.axes.x != 0 ? event.axes.x : arena.lastAxesPlayer.x, event.axes.y != 0 ? event.axes.y : arena.lastAxesPlayer.y };
            *arena.playerOrientation = arena.lastAxesPlayer.x > 0;

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
};
