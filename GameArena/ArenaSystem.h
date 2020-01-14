#pragma once
#include <filesystem>
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerComponent.h"
#include "Constants.h"
#include "NetworkEvent.h"
#include <iostream>

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

		Rectangle player_rec{ ARENA_BORDER,SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2, CHARACTER_PLACEHOLDER_WIDTH,CHARACTER_PLACEHOLDER_HEIGHT };

		auto sprite_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "basesprite.png").string();

		auto& transform = arena.player->Add<TransformComponent>(player_rec);
		arena.player->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load(sprite_path), Color(ORANGE), Rectangle{ 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT });
		arena.player->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z);

		auto& playerBody = arena.player->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, player_rec.x + player_rec.width / 2, player_rec.y + player_rec.height / 2, CHARACTER_WIDTH, CHARACTER_HEIGHT, 1).body;
		playerBody->staticFriction = 0.2f;
		playerBody->dynamicFriction = 0.2f;
		playerBody->freezeOrient = true;

		*arena.playerOrientation = true;
		arena.lastAxesPlayer = { 1, 0 };

		//Player Animation
		std::shared_ptr<AnimationNode> player_idle = std::make_shared<AnimationNode>(
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

		std::shared_ptr<AnimationNode> player_move = std::make_shared<AnimationNode>(
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

		std::shared_ptr<AnimationNode> player_attack_x = std::make_shared<AnimationNode>(
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

		std::shared_ptr<AnimationNode> player_attack_z = std::make_shared<AnimationNode>(
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
		player_idle->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
		}, &arena);
		player_move->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
		}, &arena);

		//idle, move - attack_x
		player_idle->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
		}, &arena);
		player_move->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
		}, &arena);

		player_attack_x->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.currentActionPlayer = ArenaGameComponent::IDLE;
				arenaCtx.blockPlayerInput = false;
			}

			return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
		}, &arena);
		player_attack_x->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
		}, &arena);

		//idle, move, attack_x - attack_z
		player_idle->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		player_move->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		player_attack_x->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_Z;
		}, &arena);

		player_attack_z->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.currentActionPlayer = ArenaGameComponent::IDLE;
				arenaCtx.blockPlayerInput = false;
			}

			return arenaCtx.currentActionPlayer == ArenaGameComponent::IDLE;
		}, &arena);
		player_attack_z->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::MOVE;
		}, &arena);
		player_attack_z->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionPlayer == ArenaGameComponent::ATTACK_X;
		}, &arena);

		arena.player->Add<AnimationComponent>(AnimationGraph(player_idle));

		//Player HitBox
		Shape mainBody("mainBodyP",Shape::ShapeType::RECTANGLE);
		mainBody.rectangle.x = arena.player->Get<TransformComponent>().position.x + (CHARACTER_PLACEHOLDER_WIDTH - CHARACTER_WIDTH) / 2;
		mainBody.rectangle.y = arena.player->Get<TransformComponent>().position.y + 20;
		mainBody.rectangle.width = CHARACTER_WIDTH;
		mainBody.rectangle.height = CHARACTER_HEIGHT - 20;
		mainBody.rotation = 0;

		ShapeContainer idle_cont("idle", mainBody, Vector2{ 0, 0 });

		auto running_cont = idle_cont;
		running_cont.name = "move";
		running_cont.origin_position.rectangle.height -= 10;
		running_cont.origin_position.rectangle.y += 10;

		auto attack_x_cont = idle_cont;
		attack_x_cont.name = "attack_x2";

		Shape fist("p_fist", Shape::ShapeType::RECTANGLE);
		fist.rectangle.x = arena.player->Get<TransformComponent>().position.x + 10;
		fist.rectangle.y = arena.player->Get<TransformComponent>().position.y + CHARACTER_HEIGHT / 2 + 5;
		fist.rectangle.width = 40;
		fist.rectangle.height = 40;
		fist.rotation = 0;

		fist.SetMainBodyCenter(attack_x_cont.origin_position);
		attack_x_cont.AddShape(fist);

		std::vector<ShapeContainer> s_containers;
		s_containers.push_back(idle_cont);
		s_containers.push_back(running_cont);
		s_containers.push_back(attack_x_cont);

		idle_cont.Update();
		running_cont.Update();
		attack_x_cont.Update();
		auto& player_box = arena.player->Add<HitBoxComponent>(s_containers, Vector2{ arena.player->Get<TransformComponent>().position.x , arena.player->Get<TransformComponent>().position.y });

		arena.generatedEntities.push_back(arena.player);

		//Enemy Setup
		Rectangle enemy_rec{ SCREEN_WIDTH - ARENA_BORDER - CHARACTER_WIDTH,SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2,CHARACTER_PLACEHOLDER_WIDTH,CHARACTER_PLACEHOLDER_HEIGHT };

		arena.enemy = pool->AddEntity();
		arena.enemy->Add<TransformComponent>(enemy_rec);
		arena.enemy->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load(sprite_path), Color(BLUE), Rectangle{ 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT });

		auto& enemyBody = arena.enemy->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, enemy_rec.x + enemy_rec.width / 2, enemy_rec.y + enemy_rec.height / 2, CHARACTER_WIDTH, CHARACTER_HEIGHT, 1).body;
		enemyBody->staticFriction = 0.2f;
		enemyBody->dynamicFriction = 0.2f;
		enemyBody->freezeOrient = true;

		//Enemy Animation
		std::shared_ptr<AnimationNode> enemy_idle = std::make_shared<AnimationNode>(
			AnimationUnit(
				std::string("idle"),
				textureManager->Load(sprite_path),
				Rectangle{ 0,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
				1,
				1,
				arena.enemyOrientation,
				std::make_shared<bool>(false),
				0)
			);

		std::shared_ptr<AnimationNode> enemy_move = std::make_shared<AnimationNode>(
			AnimationUnit(
				std::string("move"),
				textureManager->Load(sprite_path),
				Rectangle{ (float)SPRITE_WIDTH,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
				MOVE_ANIM_FRAMES,
				MOVE_ANIM_TIME / MOVE_ANIM_FRAMES,
				arena.enemyOrientation,
				arena.enemyOrientation,
				0
			)
			);

		std::shared_ptr<AnimationNode> enemy_attack_x = std::make_shared<AnimationNode>(
			AnimationUnit(
				std::string("attack_x"),
				textureManager->Load(sprite_path),
				Rectangle{ (float)SPRITE_WIDTH * 5,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
				ATTACK_X_ANIM_FRAMES,
				ATTACK_X_ANIM_TIME / ATTACK_X_ANIM_FRAMES,
				arena.enemyOrientation,
				std::make_shared<bool>(false),
				1)
			);

		std::shared_ptr<AnimationNode> enemy_attack_z = std::make_shared<AnimationNode>(
			AnimationUnit(
				std::string("attack_y"),
				textureManager->Load(sprite_path),
				Rectangle{ (float)SPRITE_WIDTH * 8,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
				ATTACK_Y_ANIM_FRAMES,
				ATTACK_Y_ANIM_TIME / ATTACK_Y_ANIM_FRAMES,
				arena.enemyOrientation,
				std::make_shared<bool>(false),
				1)
			);

		//idle-move
		enemy_idle->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::MOVE;
		}, &arena);
		enemy_move->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::IDLE;
		}, &arena);

		//idle, move - attack_x
		enemy_idle->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_X;
		}, &arena);
		enemy_move->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_X;
		}, &arena);

		enemy_attack_x->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.currentActionEnemy = ArenaGameComponent::IDLE;
				arenaCtx.blockEnemyInput = false;
			}

			return arenaCtx.currentActionEnemy == ArenaGameComponent::IDLE;
		}, &arena);
		enemy_attack_x->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::MOVE;
		}, &arena);

		//idle, move, attack_x - attack_z
		enemy_idle->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		enemy_move->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		enemy_attack_x->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_Z;
		}, &arena);

		enemy_attack_z->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.currentActionEnemy = ArenaGameComponent::IDLE;
				arenaCtx.blockEnemyInput = false;
			}

			return arenaCtx.currentActionEnemy == ArenaGameComponent::IDLE;
		}, &arena);
		enemy_attack_z->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::MOVE;
		}, &arena);
		enemy_attack_z->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.currentActionEnemy == ArenaGameComponent::ATTACK_X;
		}, &arena);

		arena.enemy->Add<AnimationComponent>(AnimationGraph(enemy_idle));

		//Enemy HitBox
		Shape mainBodyE("mainBodyE",Shape::ShapeType::RECTANGLE);
		mainBodyE.rectangle.x = arena.enemy->Get<TransformComponent>().position.x + (CHARACTER_PLACEHOLDER_WIDTH - CHARACTER_WIDTH) / 2;
		mainBodyE.rectangle.y = arena.enemy->Get<TransformComponent>().position.y + 20;
		mainBodyE.rectangle.width = CHARACTER_WIDTH;
		mainBodyE.rectangle.height = CHARACTER_HEIGHT - 20;
		mainBodyE.rotation = 0;

		ShapeContainer e_idle_cont("idle", mainBodyE, Vector2{ 0, 0 });

		auto e_running_cont = e_idle_cont;
		e_running_cont.name = "move";
		e_running_cont.origin_position.rectangle.height -= 10;
		e_running_cont.origin_position.rectangle.y += 10;

		auto e_attack_x_cont = e_idle_cont;
		e_attack_x_cont.name = "attack_x2";

		Shape e_fist("e_fist", Shape::ShapeType::RECTANGLE);
		e_fist.rectangle.x = arena.enemy->Get<TransformComponent>().position.x + 10;
		e_fist.rectangle.y = arena.enemy->Get<TransformComponent>().position.y + CHARACTER_HEIGHT / 2 + 5;
		e_fist.rectangle.width = 40;
		e_fist.rectangle.height = 40;
		e_fist.rotation = 0;

		e_fist.SetMainBodyCenter(e_attack_x_cont.origin_position);
		e_attack_x_cont.AddShape(fist);

		std::vector<ShapeContainer> e_containers;
		e_containers.push_back(e_idle_cont);
		e_containers.push_back(e_running_cont);
		e_containers.push_back(e_attack_x_cont);

		e_idle_cont.Update();
		e_running_cont.Update();
		e_attack_x_cont.Update();
		auto& enemy_box = arena.enemy->Add<HitBoxComponent>(e_containers, Vector2{ arena.enemy->Get<TransformComponent>().position.x , arena.enemy->Get<TransformComponent>().position.y });

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

			box.Mirror(Vector2{ float(*arena.playerOrientation), 0 });
			box.Update();

			eventManager->Notify<NetworkEvent>(NetworkEvent::SEND, nlohmann::json{
					{"head", "player_keyboard_event"},
					{"pressed_keys", te.pressedKeys},
					{"held_keys", te.heldKeys}
				}
			);

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
		}
	}

    void Receive(const HitBoxEvent& event)
    {
        for (auto& info : event.allTriggerInfos)
        {
			auto& h = info.e1->Get<HealthComponent>();
        }
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

					nlohmann::json j = nlohmann::json::parse(&p[0], nullptr, false);
		        	if (j.is_discarded())
						continue;
		        	
					auto pressedKeys = j.at("pressed_keys").get<std::vector<int>>();
					auto heldKeys = j.at("held_keys").get<std::vector<int>>();

					for (auto& e : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>())) {

						auto& arena =e->Get<ArenaGameComponent>();
						auto& comp = arena.enemy->Get<PhysicsComponent>();
						auto& box = arena.enemy->Get<HitBoxComponent>();

						//Move
						const float x = ((std::find(pressedKeys.begin(), pressedKeys.end(), KEY_RIGHT) != pressedKeys.end())
							- (std::find(pressedKeys.begin(), pressedKeys.end(), KEY_LEFT) != pressedKeys.end()));
						const float y = ((std::find(pressedKeys.begin(), pressedKeys.end(), KEY_DOWN) != pressedKeys.end())
							- (std::find(pressedKeys.begin(), pressedKeys.end(), KEY_UP) != pressedKeys.end()));

						const float hX = ((std::find(heldKeys.begin(), heldKeys.end(), KEY_RIGHT) != heldKeys.end())
							- (std::find(heldKeys.begin(), heldKeys.end(), KEY_LEFT) != heldKeys.end()));
						const float hY = ((std::find(heldKeys.begin(), heldKeys.end(), KEY_DOWN) != heldKeys.end())
							- (std::find(heldKeys.begin(), heldKeys.end(), KEY_UP) != heldKeys.end()));

						Vector2 axes{ -x + -hX, y + hY };

						arena.lastAxesEnemy = { axes.x != 0 ? axes.x : arena.lastAxesEnemy.x, axes.y != 0 ? axes.y : arena.lastAxesEnemy.y };
						*arena.enemyOrientation = arena.lastAxesEnemy.x > 0;

						box.Mirror(Vector2{ float(*arena.enemyOrientation), 0 });
						box.Update();

						if (arena.blockEnemyInput)
							continue;

						//Action
						if (std::find(pressedKeys.begin(), pressedKeys.end(), KEY_X) != pressedKeys.end())
						{
							arena.currentActionEnemy = ArenaGameComponent::ATTACK_X;
							arena.blockEnemyInput = true;
							comp.body->velocity = { 0,0 };
						}
						else if (std::find(pressedKeys.begin(), pressedKeys.end(), KEY_Z) != pressedKeys.end())
						{
							arena.currentActionEnemy = ArenaGameComponent::ATTACK_Z;
							arena.blockEnemyInput = true;
							comp.body->velocity = { 0,0 };
						}
						else {

							comp.body->velocity = { axes.x * VELOCITY , axes.y * VELOCITY };

							if (axes == Vector2{ 0, 0 }) {
								arena.currentActionEnemy = ArenaGameComponent::IDLE;
							}
							else {
								arena.currentActionEnemy = ArenaGameComponent::MOVE;
							}
						}
					}
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

	void Receive(const AnimationEvent& event)
    {
		if (event.entity->Has<HitBoxComponent>())
		{
			auto& box = event.entity->Get<HitBoxComponent>();

			for (auto& b : box.containers)
			{
				if (box.current_container->name != b.name && (b.name == event.node.animationUnit->name || b.name == event.node.animationUnit->name + std::to_string(event.node.animationUnit->currentFrame)))
				{
					b.origin_position.SetCenterX(box.current_container->origin_position.GetCenterX());
					b.origin_position.SetCenterY(box.current_container->origin_position.GetCenterY());
					box.current_container = &b;
					box.current_container->Update();
				}
			}

			
		}
    }
};
