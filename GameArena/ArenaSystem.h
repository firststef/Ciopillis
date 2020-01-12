#pragma once
#include <filesystem>
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerComponent.h"
#include "Constants.h"
#include "NetworkEvent.h"

class ArenaSystem : public ISystem
{
	bool connected_with_server = false;
	
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
		arena.player->Add<ArenaPlayerComponent>(e);

        Rectangle player_rec{ 1000,500,200,200 };

		auto sprite_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "basesprite.png").string();

        auto& transform = arena.player->Add<TransformComponent>(player_rec);
        arena.player->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load(sprite_path), Color(ORANGE), Rectangle{ 0, 0, 29, 24});
        arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z);
        
        auto& playerBody = arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, player_rec.x, player_rec.y, 100, player_rec.height, 1).body;
        playerBody->staticFriction = 0.2f;
        playerBody->dynamicFriction = 0.2f;
        playerBody->freezeOrient = false;

        //Player Animation
        std::shared_ptr<AnimationNode> idle = std::make_shared<AnimationNode>(
            AnimationUnit(
                std::string("idle"),
                textureManager->Load(sprite_path),
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
                textureManager->Load(sprite_path),
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
                textureManager->Load(sprite_path),
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
                textureManager->Load(sprite_path),
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

		Shape mainBody;
		mainBody.type = Shape::ShapeType::RECTANGLE;
		mainBody.rectangle.center.x = arena.player->Get<TransformComponent>().position.x;
		mainBody.rectangle.center.y = arena.player->Get<TransformComponent>().position.y;
		mainBody.rectangle.width = 100;
		mainBody.rectangle.height = 200;
		mainBody.rotation = 0;

		ShapeContainer player_idle_cont(mainBody, 0.0f);
    	
        //Player HitBox

        //Shape body("body", "player");
        //body.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(BLUE, 0.4f));
        //auto& b = player_idle_cont.AddShape(body, Vector2{ -100,0 }, Vector2{ -1, 1 }, false);
		//body_ptr = &b;

		AttachedShape fist;
		fist.type = Shape::ShapeType::RECTANGLE;
		fist.rectangle.center.x = arena.player->Get<TransformComponent>().position.x + 30;
		fist.rectangle.center.y = arena.player->Get<TransformComponent>().position.y;
		fist.rectangle.width = 40;
		fist.rectangle.height = 40;
		fist.rotation = 0;

        //Shape fist("fist", "player");
        //fist.SetRectangle(Rectangle{ 0,0,20,20 }, 0.0f, Fade(RED, 0.4f));
        //player_idle_cont.AddShape(fist, Vector2{ 0,0 }, Vector2{ 1, 1 }, false);

		fist.SetMainBodyCenter(mainBody);
		player_idle_cont.AddShape(fist);

        arena.player->Add<HitBoxComponent>(player_idle_cont);
        player_idle_cont.Update();

        arena.generatedEntities.push_back(arena.player);

        //Enemy Setup

        Rectangle enemy_rec{ 200,200,200,200 };

        arena.enemy = pool->AddEntity();
        arena.enemy->Add<TransformComponent>(enemy_rec);
        arena.enemy->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load(sprite_path), Color(BLUE), Rectangle{ 0, 0, 29, 24 });
        
        auto& enemyBody = arena.enemy->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, enemy_rec.x, enemy_rec.y, 100, enemy_rec.height, 1).body;
        enemyBody->staticFriction = 0.2f;
        enemyBody->dynamicFriction = 0.2f;
        enemyBody->freezeOrient = true;

        //Enemy hitboxlayer_idle
		/*ShapeContainer enemy_idle_cont(
			arena.enemy->Get<TransformComponent>().position,
			arena.enemy->Get<PhysicsComponent>().body->orient
		);

        Shape enemy_body("body", "enemy");
        enemy_body.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(BLUE, 0.4f));
        enemy_idle_cont.AddShape(enemy_body, Vector2{ -100,0 }, Vector2{ -1, 1 }, false);

        /*Shape enemy_fist("fist", "enemy");
        enemy_fist.SetRectangle(Rectangle{ 0,0,100,200 }, 0.0f, Fade(RED, 0.4f));
        enemy_idle_cont.AddShape(Vector2{ 0,0 }, enemy_fist, false);

        arena.enemy->Add<HitBoxComponent>(enemy_idle_cont);
        enemy_idle_cont.Update();*/

        arena.generatedEntities.push_back(arena.enemy);

        arena.state = ArenaGameComponent::RUNNING;
    }

    void OnRunning(EntityPtr e)
    {
        //Height sorting
    	//TODO: this might be the job for another system
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

		//if (! body_ptr)
		//	return;
    	
		//auto body = *(ShapeContainer::ShapeHolder*)(body_ptr);
		//printf("Player position %f %f\n", arena.player->Get<TransformComponent>().position.x, arena.player->Get<TransformComponent>().position.y);
		//printf("Player rotation %f rad %f\n", arena.player->Get<TransformComponent>().rotation, arena.player->Get<PhysicsComponent>().body->orient);
		//printf("Shape position %f %f\n", body.shape.rectangle.rec.x, body.shape.rectangle.rec.y);
		//printf("Shape rotation %f\n", body.shape.rotation);
		//printf("Shape offset %f", sqrt(pow(arena.player->Get<TransformComponent>().position.x - body.shape.rectangle.rec.x, 2)
		//	+ pow(arena.player->Get<TransformComponent>().position.y - body.shape.rectangle.rec.y, 2)));
		//system("CLS");
    }

    void OnEnd(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        arena.generatedEntities.clear();
    }

public:
    ArenaSystem() : ISystem(std::string("ArenaSystem")) {}

    void Initialize() override {}

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

	void Receive(const KeyboardEvent& event)
	{
		//TODO: aici trebuie verificat daca jucatorul tine apasat pe hold, daca a trecut timpul de cooldown

		for (auto& te : event.triggered_entities) {

			if (!te.entity->Has<ArenaPlayerComponent>())
				continue;

			auto& apc = te.entity->Get<ArenaPlayerComponent>();
			auto& arena = apc.arena->Get<ArenaGameComponent>();
			auto& comp = arena.player->Get<PhysicsComponent>();
			auto& box = arena.player->Get<HitBoxComponent>();

			//Move
			const float x = ((std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_RIGHT) != te.pressedKeys.end())
				- (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_LEFT) != te.pressedKeys.end()));
			const float y = ((std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_DOWN) != te.pressedKeys.end())
				- (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_UP) != te.pressedKeys.end()));

			const float hX = ((std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_RIGHT) != te.heldKeys.end())
				- (std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_LEFT) != te.heldKeys.end()));
			const float hY = ((std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_DOWN) != te.heldKeys.end())
				- (std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_UP) != te.heldKeys.end()));

			Vector2 axes{ x + hX, y + hY };

			arena.lastAxesPlayer = { axes.x != 0 ? axes.x : arena.lastAxesPlayer.x, axes.y != 0 ? axes.y : arena.lastAxesPlayer.y };
			*arena.playerOrientation = arena.lastAxesPlayer.x > 0;

			//box.cont.Mirror(Vector2{ arena.lastAxesPlayer.x, 1 });
			//box.cont.Update();

			if (arena.blockPlayerInput)
				continue;

			ArenaGameComponent::CurrentAction action;

			//Action
			if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_X) != te.pressedKeys.end())
			{
				arena.currentActionPlayer = ArenaGameComponent::ATTACK_X;
				action = ArenaGameComponent::ATTACK_X;
				arena.blockPlayerInput = true;
				comp.body->velocity = { 0,0 };
			}
			else if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_Z) != te.pressedKeys.end())
			{
				arena.currentActionPlayer = ArenaGameComponent::ATTACK_Z;
				action = ArenaGameComponent::ATTACK_X;
				arena.blockPlayerInput = true;
				comp.body->velocity = { 0,0 };
			}
			else {

				comp.body->velocity = { axes.x * VELOCITY , axes.y * VELOCITY };

				if (axes == Vector2{ 0, 0 }) {
					arena.currentActionPlayer = ArenaGameComponent::IDLE;
					action = ArenaGameComponent::ATTACK_X;
				}
				else {
					arena.currentActionPlayer = ArenaGameComponent::MOVE;
					action = ArenaGameComponent::ATTACK_X;
				}
			}

			eventManager->Notify<NetworkEvent>(NetworkEvent::SEND, nlohmann::json{
				{"head", "player_keyboard_event"},
				{"pressed_keys", nlohmann::json::array({te.pressedKeys})},
				{"held_keys", nlohmann::json::array({te.heldKeys})}
			}
				);
		}
	}

    void Receive(const HitBoxEvent& event)
    {
        auto test = 3;
    }

	void Receive(const NetworkEvent& event)
    {
    	if(connected_with_server)
        {
	        if (event.type == NetworkEvent::RECEIVE)
	        {
		        for (auto& p : event.packets)
		        {
			        printf("p: %s\n", &p[0]);
		        }
	        }
        }
		else
        {
	        eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "KeyboardInputSystem");
	        eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "PhysicsSystem");
	        eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "AnimationSystem");
	        eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "HitBoxSystem");

	        connected_with_server = true;
        }
    }
};
