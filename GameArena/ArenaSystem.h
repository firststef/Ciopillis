#pragma once
#include "System.h"
#include "ArenaGameComponent.h"
#include "ArenaPlayerComponent.h"
#include "ArenaEnemyComponent.h"
#include "Constants.h"
#include "NetworkEvent.h"
#include <filesystem>
#include <iostream>

class ArenaSystem : public ISystem
{
	bool connected_with_server = false;
	EntityPtr displayScreen = nullptr;
	
	void OnInit(EntityPtr e)
	{
		auto& arena = e->Get<ArenaGameComponent>();

		auto sprite_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "basesprite.png").string();
		auto target_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "target.png").string();
		auto background_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "background.png").string();

		//Background Setup
		auto fadedBackground(pool->AddEntity());
		fadedBackground->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT });
		fadedBackground->Add<SpriteComponent>(std::string("FadedBackground"), textureManager->Load(background_path), WHITE);
		arena.generatedEntities.push_back(fadedBackground);

		auto wall_left = pool->AddEntity();
		auto& body_wl = wall_left->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, -3, SCREEN_HEIGHT / 2, 6, SCREEN_HEIGHT, 1).body;
		body_wl->enabled = false;
		auto wall_up = pool->AddEntity();
		auto& body_wu = wall_left->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, SCREEN_WIDTH / 2, -4, SCREEN_WIDTH , 6, 1).body;
		body_wu->enabled = false;
		auto wall_right = pool->AddEntity();
		auto& body_wr = wall_left->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, SCREEN_WIDTH + 4, SCREEN_HEIGHT / 2, 6, SCREEN_HEIGHT, 1).body;
		body_wr->enabled = false;
		auto wall_down = pool->AddEntity();
		auto& body_wd = wall_left->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, SCREEN_WIDTH / 2, SCREEN_HEIGHT + 3, SCREEN_WIDTH, 6, 1).body;
		body_wd->enabled = false;
		arena.generatedEntities.push_back(wall_left);
		arena.generatedEntities.push_back(wall_up);
		arena.generatedEntities.push_back(wall_right);
		arena.generatedEntities.push_back(wall_down);

		////////////////////////////////////////////////////////////////////Player Setup
		arena.player.ptr = pool->AddEntity();
		arena.player.ptr->Add<ArenaPlayerComponent>(e);

		Rectangle player_rec{ ARENA_BORDER,SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2, CHARACTER_PLACEHOLDER_WIDTH,CHARACTER_PLACEHOLDER_HEIGHT };

		auto& transform = arena.player.ptr->Add<TransformComponent>(player_rec);
		arena.player.ptr->Add<SpriteComponent>(std::string("Fighter"), textureManager->Load(sprite_path), Color(WHITE), Rectangle{ 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT });
		arena.player.ptr->Add<KeyboardInputComponent>(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_X, KEY_Z);

		auto& playerBody = arena.player.ptr->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, player_rec.x + player_rec.width / 2, player_rec.y + player_rec.height / 2, CHARACTER_WIDTH, CHARACTER_HEIGHT, 1).body;
		playerBody->staticFriction = 0.2f;
		playerBody->dynamicFriction = 0.2f;
		playerBody->freezeOrient = true;

		*arena.player.orientation = true;
		arena.player.lastAxes = { 1, 0 };

		//Player Animation
		std::shared_ptr<AnimationNode> player_idle = std::make_shared<AnimationNode>(
			AnimationUnit(
				std::string("idle"),
				textureManager->Load(sprite_path),
				Rectangle{ 0,0,(float)SPRITE_WIDTH, (float)SPRITE_HEIGHT },
				1,
				1,
				arena.player.orientation,
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
				arena.player.orientation,
				arena.player.orientation,
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
				arena.player.orientation,
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
				arena.player.orientation,
				std::make_shared<bool>(false),
				1)
			);

		//idle-move
		player_idle->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::MOVE;
		}, &arena);
		player_move->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::IDLE;
		}, &arena);

		//idle, move - attack_x
		player_idle->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);
		player_move->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);

		player_attack_x->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.player.currentAction = ArenaGameComponent::IDLE;
				arenaCtx.player.blockInput = false;
			}

			return arenaCtx.player.currentAction == ArenaGameComponent::IDLE;
		}, &arena);
		player_attack_x->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::MOVE;
		}, &arena);

		//idle, move, attack_x - attack_z
		player_idle->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		player_move->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		player_attack_x->Next(player_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);

		player_attack_z->Next(player_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.player.currentAction = ArenaGameComponent::IDLE;
				arenaCtx.player.blockInput = false;
			}

			return arenaCtx.player.currentAction == ArenaGameComponent::IDLE;
		}, &arena);
		player_attack_z->Next(player_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::MOVE;
		}, &arena);
		player_attack_z->Next(player_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.player.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);

		arena.player.ptr->Add<AnimationComponent>(AnimationGraph(player_idle));

		//Player HitBox
		Shape mainBody("mainBodyP",Shape::ShapeType::RECTANGLE);
		mainBody.rectangle.x = arena.player.ptr->Get<TransformComponent>().position.x + (CHARACTER_PLACEHOLDER_WIDTH - CHARACTER_WIDTH) / 2;
		mainBody.rectangle.y = arena.player.ptr->Get<TransformComponent>().position.y + 20;
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
		fist.rectangle.x = arena.player.ptr->Get<TransformComponent>().position.x + 10;
		fist.rectangle.y = arena.player.ptr->Get<TransformComponent>().position.y + CHARACTER_HEIGHT / 2 + 5;
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
		auto& player_box = arena.player.ptr->Add<HitBoxComponent>(s_containers, Vector2{ arena.player.ptr->Get<TransformComponent>().position.x , arena.player.ptr->Get<TransformComponent>().position.y });

		//Target Point Player
		arena.player.target = pool->AddEntity();

		arena.player.target->Add<TransformComponent>(Rectangle{OUT_OF_BOUNDS_X, OUT_OF_BOUNDS_Y, TARGET_WIDTH, TARGET_HEIGHT});
		arena.player.target->Add<SpriteComponent>(std::string("PlayerTarget"), textureManager->Load(target_path), Color(WHITE), Rectangle{ 0, 0, TARGET_SPRITE_WIDTH, TARGET_SPRITE_HEIGHT });

		Shape targetPPlayer("targetPPlayer", Shape::ShapeType::RECTANGLE);
		targetPPlayer.rectangle.x = arena.player.target->Get<TransformComponent>().position.x + arena.player.target->Get<TransformComponent>().position.width / 2 + 25;
		targetPPlayer.rectangle.y = arena.player.target->Get<TransformComponent>().position.y + arena.player.target->Get<TransformComponent>().position.height / 2 + 25;
		targetPPlayer.rectangle.width = TARGET_WIDTH - 50;
		targetPPlayer.rectangle.height = TARGET_HEIGHT - 50;

		ShapeContainer targetPPlayerCont("explode", targetPPlayer, Vector2{ 0, 0 });
		targetPPlayerCont.Update();
		
		std::vector<ShapeContainer> ptargetvec = { targetPPlayerCont };
		
		auto& tbox = arena.player.target->Add<HitBoxComponent>(ptargetvec, 
			Vector2{
				arena.player.target->Get<TransformComponent>().position.x + (arena.player.target->Get<TransformComponent>().position.width / 2),
				arena.player.target->Get<TransformComponent>().position.y + (arena.player.target->Get<TransformComponent>().position.height / 2)
			});
		tbox.current_container = nullptr;

		arena.generatedEntities.push_back(arena.player.target);
		
		//Player Stats
		arena.player.ptr->Add<CharacterStatsComponent>(BASE_HP, BASE_VELOCITY);

		//Player LifeBox
		arena.player.lifeBox = pool->AddEntity();
		arena.player.lifeBox->Add<TransformComponent>(Rectangle{ HEALTH_BAR_PADDING, HEALTH_BAR_PADDING, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT });
		arena.player.lifeBox->Add<SpriteComponent>(std::string("LifeBoxPlayer"), Texture2D({ 0 }), RED, Rectangle{ HEALTH_BAR_PADDING, HEALTH_BAR_PADDING, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT });

		arena.generatedEntities.push_back(arena.player.ptr);

		///////////////////////////////////////////////////////////////Enemy Setup
		arena.enemy.ptr = pool->AddEntity();
		arena.enemy.ptr->Add<ArenaEnemyComponent>(e);

		Rectangle enemy_rec{ SCREEN_WIDTH - ARENA_BORDER - CHARACTER_WIDTH,SCREEN_HEIGHT / 2 - CHARACTER_HEIGHT / 2,CHARACTER_PLACEHOLDER_WIDTH,CHARACTER_PLACEHOLDER_HEIGHT };
		arena.enemy.ptr->Add<TransformComponent>(enemy_rec);
		arena.enemy.ptr->Add<SpriteComponent>(std::string("Enemy"), textureManager->Load(sprite_path), Color(WHITE), Rectangle{ 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT });

		auto& enemyBody = arena.enemy.ptr->Add<PhysicsComponent>(PhysicsComponent::RECTANGLE, enemy_rec.x + enemy_rec.width / 2, enemy_rec.y + enemy_rec.height / 2, CHARACTER_WIDTH, CHARACTER_HEIGHT, 1).body;
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
				arena.enemy.orientation,
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
				arena.enemy.orientation,
				arena.enemy.orientation,
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
				arena.enemy.orientation,
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
				arena.enemy.orientation,
				std::make_shared<bool>(false),
				1)
			);

		//idle-move
		enemy_idle->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::MOVE;
		}, &arena);
		enemy_move->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::IDLE;
		}, &arena);

		//idle, move - attack_x
		enemy_idle->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);
		enemy_move->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);

		enemy_attack_x->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.enemy.currentAction = ArenaGameComponent::IDLE;
				arenaCtx.enemy.blockInput = false;
			}

			return arenaCtx.enemy.currentAction == ArenaGameComponent::IDLE;
		}, &arena);
		enemy_attack_x->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::MOVE;
		}, &arena);

		//idle, move, attack_x - attack_z
		enemy_idle->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		enemy_move->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);
		enemy_attack_x->Next(enemy_attack_z, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_Z;
		}, &arena);

		enemy_attack_z->Next(enemy_idle, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			if (node.animationUnit->currentRepeat == 1)
			{
				arenaCtx.enemy.currentAction = ArenaGameComponent::IDLE;
				arenaCtx.enemy.blockInput = false;
			}

			return arenaCtx.enemy.currentAction == ArenaGameComponent::IDLE;
		}, &arena);
		enemy_attack_z->Next(enemy_move, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::MOVE;
		}, &arena);
		enemy_attack_z->Next(enemy_attack_x, [](const AnimationNode& node, void* context) ->bool
		{
			auto& arenaCtx = *static_cast<ArenaGameComponent*>(context);

			return arenaCtx.enemy.currentAction == ArenaGameComponent::ATTACK_X;
		}, &arena);

		arena.enemy.ptr->Add<AnimationComponent>(AnimationGraph(enemy_idle));

		//Enemy HitBox
		Shape mainBodyE("mainBodyE",Shape::ShapeType::RECTANGLE);
		mainBodyE.rectangle.x = arena.enemy.ptr->Get<TransformComponent>().position.x + (CHARACTER_PLACEHOLDER_WIDTH - CHARACTER_WIDTH) / 2;
		mainBodyE.rectangle.y = arena.enemy.ptr->Get<TransformComponent>().position.y + 20;
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
		e_fist.rectangle.x = arena.enemy.ptr->Get<TransformComponent>().position.x + 10;
		e_fist.rectangle.y = arena.enemy.ptr->Get<TransformComponent>().position.y + CHARACTER_HEIGHT / 2 + 5;
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
		auto& enemy_box = arena.enemy.ptr->Add<HitBoxComponent>(e_containers, Vector2{ arena.enemy.ptr->Get<TransformComponent>().position.x , arena.enemy.ptr->Get<TransformComponent>().position.y });

		// Target Enemy
		arena.enemy.target = pool->AddEntity();

		arena.enemy.target->Add<TransformComponent>(Rectangle{OUT_OF_BOUNDS_X, OUT_OF_BOUNDS_Y, TARGET_WIDTH, TARGET_HEIGHT});
		arena.enemy.target->Add<SpriteComponent>(std::string("EnemyTarget"), textureManager->Load(target_path), Color(WHITE), Rectangle{ 0, 0, TARGET_SPRITE_WIDTH, TARGET_SPRITE_HEIGHT });

		Shape targetPEnemy("targetPEnemy", Shape::ShapeType::RECTANGLE);
		targetPEnemy.rectangle.x = arena.enemy.target->Get<TransformComponent>().position.x + arena.enemy.target->Get<TransformComponent>().position.width / 2 + 25;
		targetPEnemy.rectangle.y = arena.enemy.target->Get<TransformComponent>().position.y + arena.enemy.target->Get<TransformComponent>().position.height / 2 + 25;
		targetPEnemy.rectangle.width = TARGET_WIDTH - 50;
		targetPEnemy.rectangle.height = TARGET_HEIGHT - 50;

		ShapeContainer targetPEnemyCont("explode", targetPEnemy, Vector2{ 0, 0 });
		targetPEnemyCont.Update();
		
		std::vector<ShapeContainer> etargetvec = { targetPEnemyCont };
		
		auto& e_tbox = arena.enemy.target->Add<HitBoxComponent>(etargetvec, 
			Vector2{
				arena.enemy.target->Get<TransformComponent>().position.x + (arena.enemy.target->Get<TransformComponent>().position.width / 2),
				arena.enemy.target->Get<TransformComponent>().position.y + (arena.enemy.target->Get<TransformComponent>().position.height / 2)
			});
		e_tbox.current_container = nullptr;

		arena.generatedEntities.push_back(arena.enemy.target);

		//Enemy Stats
		arena.enemy.ptr->Add<CharacterStatsComponent>(BASE_HP, BASE_VELOCITY);

		//Enemy lifebox
		arena.enemy.lifeBox = pool->AddEntity();
		arena.enemy.lifeBox->Add<TransformComponent>(Rectangle{ SCREEN_WIDTH - HEALTH_BAR_PADDING - HEALTH_BAR_WIDTH, HEALTH_BAR_PADDING, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT });
		arena.enemy.lifeBox->Add<SpriteComponent>(std::string("LifeBoxEnemy"), Texture2D({0}), RED, Rectangle{ SCREEN_WIDTH - HEALTH_BAR_PADDING - HEALTH_BAR_WIDTH, HEALTH_BAR_PADDING, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT });

		arena.generatedEntities.push_back(arena.enemy.ptr);

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

		for (auto& e : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>()))
		{
			if (type == SERVER) {

				auto& arena = e->Get<ArenaGameComponent>();
				auto& transp = arena.player.ptr->Get<TransformComponent>();
				auto& physicp = arena.player.ptr->Get<PhysicsComponent>();
				auto& attribp = arena.player;
				auto& statsp = arena.player.ptr->Get<CharacterStatsComponent>();
				auto& transe = arena.enemy.ptr->Get<TransformComponent>();
				auto& physice = arena.enemy.ptr->Get<PhysicsComponent>();
				auto& attribe = arena.enemy;
				auto& statse = arena.enemy.ptr->Get<CharacterStatsComponent>();

				bool auxTriggerTargetPlayer = false;
				if (arena.player.triggerTarget)
				{
					auxTriggerTargetPlayer = true;
					arena.player.triggerTarget = false;
				}

				bool auxTriggerTargetEnemy = false;
				if (arena.enemy.triggerTarget)
				{
					auxTriggerTargetEnemy = true;
					arena.enemy.triggerTarget = false;
				}

				eventManager->Notify<NetworkEvent>(NetworkEvent::SEND, nlohmann::json{
					{"head", "player_coordinates"},
					{"player",{
							{"x", physicp.body->position.x},
							{"y", physicp.body->position.y},
							{"vx", physicp.body->velocity.x},
							{"vy", physicp.body->velocity.y},
							{"current_action", int(arena.player.currentAction)},
							{"orientation", *arena.player.orientation},
							{"trigger_target", auxTriggerTargetPlayer},
							{"hp", statsp.hp}
						}
					},
					{"enemy",{
							{"x", SCREEN_WIDTH - physice.body->position.x},
							{"y", physice.body->position.y},
							{"vx", -physice.body->velocity.x},
							{"vy", physice.body->velocity.y},
							{"current_action", int(arena.enemy.currentAction)},
							{"orientation", !*arena.enemy.orientation},
							{"trigger_target", auxTriggerTargetEnemy},
							{"hp", statse.hp}
						}
					}
					});
			}
			else
			{
				bool stop = false;
				if (e->Get<ArenaGameComponent>().player.ptr->Get<CharacterStatsComponent>().hp <= 0)
				{
					stop = true;

					displayScreen->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT }).zIndex = 10;
					auto lose_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "lose.png").string();
					displayScreen->Add<SpriteComponent>(std::string("EndScreen"), textureManager->Load(lose_path), RAYWHITE);
				}
				else if (e->Get<ArenaGameComponent>().enemy.ptr->Get<CharacterStatsComponent>().hp <= 0)
				{
					stop = true;

					displayScreen->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT }).zIndex = 10;
					auto win_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "win.png").string();
					displayScreen->Add<SpriteComponent>(std::string("EndScreen"), textureManager->Load(win_path), RAYWHITE);
				}

				if (stop)
				{
					eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, "KeyboardInputSystem");
					eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, "PhysicsSystem");
					eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, "AnimationSystem");
					eventManager->Notify<SystemControlEvent>(SystemControlEvent::DISABLE, "HitBoxSystem");
				}
			}
		}//de reparat coordonatele inegale
		
    }

    void OnEnd(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        arena.generatedEntities.clear();
    }

	void TriggerTargetAttack(ArenaGameComponent::ArenaCharacterAttributes* characterAttributes, ArenaGameComponent::ArenaCharacterAttributes* other_character_attributes){
		//Enable target
		if (!characterAttributes->targetActive)
		{
			auto& transt = characterAttributes->target->Get<TransformComponent>();

			const auto e_rec = other_character_attributes->ptr->Get<TransformComponent>().position;

			transt.position.x = e_rec.x - transt.position.width / 2 + e_rec.width / 2;
			transt.position.y = e_rec.y - transt.position.height / 2 + e_rec.height / 2;

			auto cptr = new void*[2]{ characterAttributes, eventManager };

			std::function<void(void*)> trigger_target = [](void* context)->void
			{
				auto& boxt = static_cast<ArenaGameComponent::ArenaCharacterAttributes*>(reinterpret_cast<void**>(context)[0])->target->Get<HitBoxComponent>();
				boxt.current_container = &boxt.containers[0];
				boxt.current_container->Update();

				std::function<void(void*)> move_target = [](void* ctx)->void
				{
					auto& boxt = static_cast<ArenaGameComponent::ArenaCharacterAttributes*>(reinterpret_cast<void**>(ctx)[0])->target->Get<HitBoxComponent>();
					auto& t = static_cast<ArenaGameComponent::ArenaCharacterAttributes*>(reinterpret_cast<void**>(ctx)[0])->target->Get<TransformComponent>();
					auto& boxt2 = static_cast<ArenaGameComponent::ArenaCharacterAttributes*>(reinterpret_cast<void**>(ctx)[0])->target->Get<HitBoxComponent>();
					boxt2.current_container = &boxt2.containers[0];
					boxt2.current_container->Update();

					boxt2.current_container = nullptr;
					t.position.x = OUT_OF_BOUNDS_X;
					t.position.y = OUT_OF_BOUNDS_Y;
					static_cast<ArenaGameComponent::ArenaCharacterAttributes*>(reinterpret_cast<void**>(ctx)[0])->targetActive = false;

					delete[] reinterpret_cast<void**>(ctx);
				};

				static_cast<EventManager*>(reinterpret_cast<void**>(context)[1])->Notify<DefferEvent>(1000, move_target, context);
			};

			eventManager->Notify<DefferEvent>(1000, trigger_target, cptr);

			characterAttributes->targetActive = true;
		}
	}

public:
	enum Type {
		SERVER,
		CLIENT,
		TESTER
	} type;
	
    ArenaSystem(Type type) : ISystem(std::string("ArenaSystem")), type(type)
    {
		if (type == TESTER||type == SERVER)
			connected_with_server = true;
    }

	void Initialize() override
    {
		displayScreen = pool->AddEntity();
		displayScreen->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT });
		auto wait_screen_path = (std::filesystem::path(CIOPILLIS_ROOT) / "Resources" / "sprites" / "waitscreen.png").string();
		displayScreen->Add<SpriteComponent>(std::string("WaitingScreen"), textureManager->Load(wait_screen_path), RAYWHITE);
    }

    void Execute() override
    {
		if (!connected_with_server)
			return;
    	
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

			if (!te.entity->Has<ArenaPlayerComponent>() && !te.entity->Has<ArenaEnemyComponent>())
				continue;

			ArenaGameComponent* arenaPtr;
			bool isPlayer;
			EntityPtr triggeredEntity = nullptr;
			ArenaGameComponent::ArenaCharacterAttributes* characterAttributes = nullptr;
			ArenaGameComponent::ArenaCharacterAttributes* other_character_attributes = nullptr;
			if (te.entity->Has<ArenaPlayerComponent>())
			{
				isPlayer = true;
				triggeredEntity = te.entity;
				arenaPtr = te.entity->Get<ArenaPlayerComponent>().arena->GetPtr<ArenaGameComponent>();
				characterAttributes = &arenaPtr->player;
				other_character_attributes = &arenaPtr->enemy;
			}
			else
			{
				isPlayer = false;
				triggeredEntity = te.entity;
				arenaPtr = te.entity->Get<ArenaEnemyComponent>().arena->GetPtr<ArenaGameComponent>();
				characterAttributes = &arenaPtr->enemy;
				other_character_attributes = &arenaPtr->player;
			}
			auto& comp = triggeredEntity->Get<PhysicsComponent>();
			auto& box = triggeredEntity->Get<HitBoxComponent>();
			auto& stats = triggeredEntity->Get<CharacterStatsComponent>();

			//Move
			const float x = ((std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_RIGHT) != te.pressedKeys.end())
				- (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_LEFT) != te.pressedKeys.end()));
			const float y = ((std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_DOWN) != te.pressedKeys.end())
				- (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_UP) != te.pressedKeys.end()));

			const float hX = ((std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_RIGHT) != te.heldKeys.end())
				- (std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_LEFT) != te.heldKeys.end()));
			const float hY = ((std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_DOWN) != te.heldKeys.end())
				- (std::find(te.heldKeys.begin(), te.heldKeys.end(), KEY_UP) != te.heldKeys.end()));

			//Dash
			const auto t = time(nullptr);
			//poate ar trebui serializat si dashtime
			if (stats.state == CharacterStatsComponent::STUNNED)
			{
				characterAttributes->dashState = ArenaGameComponent::INITIAL;
			}
			else if (characterAttributes->dashState == ArenaGameComponent::INITIAL && x != 0)
			{
				characterAttributes->dashOrientation = x;
				characterAttributes->dashCounter = t;
				characterAttributes->dashState = ArenaGameComponent::NOT_READY;
			}
			else if (characterAttributes->dashState == ArenaGameComponent::NOT_READY)
			{
				if (difftime(t, characterAttributes->dashCounter) > DASH_INTERVAL || (characterAttributes->dashOrientation != x && x != 0))
				{
					characterAttributes->dashState = ArenaGameComponent::INITIAL;
				}
				else if (x == 0)
				{
					characterAttributes->dashState = ArenaGameComponent::READY;
				}
			}
			else if (characterAttributes->dashState == ArenaGameComponent::READY)
			{
				if (difftime(t, characterAttributes->dashCounter) < DASH_INTERVAL) {
					if (x != 0 && x == characterAttributes->dashOrientation)
					{
						characterAttributes->dashState = ArenaGameComponent::DASHED;
						stats.agility = BOOST_VELOCITY;
					}
				}
				else
				{
					characterAttributes->dashState = ArenaGameComponent::INITIAL;
				}
			}
			else if (characterAttributes->dashState == ArenaGameComponent::DASHED)
			{
				if (x == 0)
				{
					characterAttributes->dashState = ArenaGameComponent::INITIAL;
				}
				else
				{
					characterAttributes->dashState = ArenaGameComponent::NOT_READY;
					characterAttributes->dashOrientation = x;
				}
			}

			if (stats.state != CharacterStatsComponent::STUNNED) {
				stats.agility *= 0.85;
				stats.agility = stats.agility > stats.base_agility ? stats.agility : stats.base_agility;
			}

			Vector2 axes;
			if (isPlayer)
			{
				axes = Vector2{ x + hX, y + hY };
			}
			else
			{
				axes = Vector2{ - x - hX, y + hY };
			}

			characterAttributes->lastAxes = { axes.x != 0 ? axes.x : characterAttributes->lastAxes.x, axes.y != 0 ? axes.y : characterAttributes->lastAxes.y };
			*characterAttributes->orientation = characterAttributes->lastAxes.x > 0;

			box.Mirror(Vector2{ float(*characterAttributes->orientation), 0 });
			box.Update();

			if (type == CLIENT && isPlayer)
			{
				eventManager->Notify<NetworkEvent>(NetworkEvent::SEND, nlohmann::json{
						{"head", "player_keyboard_event"},
						{"pressed_keys", te.pressedKeys},
						{"held_keys", te.heldKeys}
					}
				);
			}

			if (characterAttributes->blockInput)
				continue;

			//Action
			if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_X) != te.pressedKeys.end())
			{
				characterAttributes->currentAction = ArenaGameComponent::ATTACK_X;
				characterAttributes->blockInput = true;
				comp.body->velocity = { 0,0 };
			}
			else if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_Z) != te.pressedKeys.end())
			{
				characterAttributes->currentAction = ArenaGameComponent::ATTACK_Z;
				characterAttributes->blockInput = true;
				comp.body->velocity = { 0,0 };

				TriggerTargetAttack(characterAttributes, other_character_attributes);

				if (type == SERVER)
				{
					characterAttributes->triggerTarget = true;
				}
			}
			else {

				comp.body->velocity = { axes.x * stats.agility , axes.y * stats.agility };

				if (axes == Vector2{ 0, 0 }) {
					characterAttributes->currentAction = ArenaGameComponent::IDLE;
				}
				else {
					characterAttributes->currentAction = ArenaGameComponent::MOVE;
				}
			}
		}
	}

    void Receive(const HitBoxEvent& event)
    {
        for (auto& info : event.allTriggerInfos)
        {
			auto ptr1 = &info.s1;
			auto ptr2 = &info.s2;

			auto t1 = info.e1->GetPtr<TransformComponent>();
			auto t2 = info.e2->GetPtr<TransformComponent>();

			auto e1 = info.e1.get();
			auto e2 = info.e2.get();

        	for (int i = 0; i <= 1; ++i, std::swap(ptr1, ptr2), std::swap(t1, t2), std::swap(e1, e2))
        	{
        		if ("fist" _in ptr1->name && "mainBody" _in ptr2->name)
        		{
					const auto velocity = BASE_VELOCITY * 3;
					const auto tg = (t1->position.y - t2->position.y) / (t1->position.x - t2->position.x);
					const auto angle = atan(tg);
        			
					e2->Get<PhysicsComponent>().body->velocity = {
						(t1->position.x < t2->position.x ? 1 : -1) * cos(angle) * velocity ,
						sin(angle) * velocity
					};

					if (type == SERVER || type == TESTER)
					{
						e2->Get<CharacterStatsComponent>().hp -= 20;

						if (e2->Has<ArenaPlayerComponent>())
						{
							auto& arena = e2->Get<ArenaPlayerComponent>().arena->Get<ArenaGameComponent>();
							arena.player.lifeBox->Get<TransformComponent>().position.width = HEALTH_BAR_WIDTH * e2->Get<CharacterStatsComponent>().hp / BASE_HP;
						}
						else 
						{
							auto& arena = e2->Get<ArenaEnemyComponent>().arena->Get<ArenaGameComponent>();
							arena.enemy.lifeBox->Get<TransformComponent>().position.width = HEALTH_BAR_WIDTH * e2->Get<CharacterStatsComponent>().hp / BASE_HP;
							arena.enemy.lifeBox->Get<TransformComponent>().position.x = SCREEN_WIDTH - HEALTH_BAR_PADDING - arena.enemy.lifeBox->Get<TransformComponent>().position.width;
						}
					}
        		}

        		if ("target" _in ptr1->name && "mainBody" _in ptr2->name)
        		{
					const auto velocity = BASE_VELOCITY * 6;
					const auto tg = (t1->position.y + t2->position.height / 2 - t2->position.y)
        			/ (t1->position.x + t1->position.width / 2 - t2->position.x);
					const auto angle = atan(tg);

					e2->Get<PhysicsComponent>().body->velocity = {0,0};

					e2->Get<CharacterStatsComponent>().state = CharacterStatsComponent::STUNNED;//add tint
					e2->Get<CharacterStatsComponent>().agility *= 0.5f;
					e2->Get<SpriteComponent>().color = RED;

					std::function<void(void*)> remove_stun = [](void* ctx)->void
					{
						auto e = (Entity*)ctx;
						auto& stats = e->Get<CharacterStatsComponent>();
						auto& s = e->Get<SpriteComponent>();

						s.color = RAYWHITE;
						stats.state = CharacterStatsComponent::NORMAL;
					};

					if (type == SERVER)
						e2->Get<CharacterStatsComponent>().hp -= 40;

					eventManager->Notify<DefferEvent>(4000, remove_stun, e2);
        		}
        	}
        }
    }

	void Receive(const NetworkEvent& event)
    {
		if (type == CLIENT && event.type == NetworkEvent::RECEIVE)
		{
			if (connected_with_server)
			{

				//for (unsigned i = 0; i < event.packets.size(); i++){printf("%d: %s\n", i, event.packets[i].data());}

				for (auto& p : event.packets)
				{
					if (p.empty())
						continue;

					std::string jstr(&p[0], p.size() + 1);
					nlohmann::json j = nlohmann::json::parse(jstr.c_str(), nullptr, false);
					if (j.is_discarded())
					{
						printf("Malformed packet%d\n", __LINE__);
						continue;
					}

					auto head = j.at("head").get<std::string>();
					if (head == "player_coordinates")
					{
						//printf("Received json %s\n", jstr.c_str());

						{
							auto x = j.at("player").at("x").get<float>();
							auto y = j.at("player").at("y").get<float>();
							auto vx = j.at("player").at("vx").get<float>();
							auto vy = j.at("player").at("vy").get<float>();
							auto current_action = j.at("player").at("current_action").get<int>();
							auto orientation = j.at("player").at("orientation").get<bool>();
							auto trigger_target = j.at("player").at("trigger_target").get<bool>();
							auto hp = j.at("player").at("hp").get<float>();

							for (auto& a : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>()))
							{
								auto& arena = a->Get<ArenaGameComponent>();
								auto& phys_e = arena.player.ptr->Get<PhysicsComponent>();

								phys_e.body->position.x = x;
								phys_e.body->position.y = y;
								phys_e.body->velocity.x = vx;
								phys_e.body->velocity.y = vy;

								arena.player.currentAction = ArenaGameComponent::CurrentAction(current_action);
								*arena.player.orientation = orientation;

								auto& box = arena.player.ptr->Get<HitBoxComponent>();
								box.Mirror(Vector2{ float(orientation), 0 });
								box.Update();

								if (trigger_target)
								{
									TriggerTargetAttack(&arena.player, &arena.enemy);
								}

								arena.player.ptr->Get<CharacterStatsComponent>().hp = hp;
								arena.player.lifeBox->Get<TransformComponent>().position.width = HEALTH_BAR_WIDTH * hp / BASE_HP;
							}
						}
						
						{
							auto x = j.at("enemy").at("x").get<float>();
							auto y = j.at("enemy").at("y").get<float>();
							auto vx = j.at("enemy").at("vx").get<float>();
							auto vy = j.at("enemy").at("vy").get<float>();
							auto current_action = j.at("enemy").at("current_action").get<int>();
							auto orientation = j.at("enemy").at("orientation").get<bool>();
							auto trigger_target = j.at("enemy").at("trigger_target").get<bool>();
							auto hp = j.at("enemy").at("hp").get<float>();
							
							for (auto& a : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>()))
							{
								auto& arena = a->Get<ArenaGameComponent>();
								auto& phys_e = arena.enemy.ptr->Get<PhysicsComponent>();

								phys_e.body->position.x = SCREEN_WIDTH - x;
								phys_e.body->position.y = y;
								phys_e.body->velocity.x = - vx;
								phys_e.body->velocity.y = vy;

								arena.enemy.currentAction = ArenaGameComponent::CurrentAction(current_action);
								*arena.enemy.orientation = !orientation;

								auto& box = arena.enemy.ptr->Get<HitBoxComponent>();
								box.Mirror(Vector2{ float(!orientation), 0 });
								box.Update();

								if (trigger_target)
								{
									TriggerTargetAttack(&arena.enemy, &arena.player);
								}

								arena.enemy.ptr->Get<CharacterStatsComponent>().hp = hp;
								arena.enemy.lifeBox->Get<TransformComponent>().position.width = HEALTH_BAR_WIDTH * hp / BASE_HP;
								arena.enemy.lifeBox->Get<TransformComponent>().position.x = SCREEN_WIDTH - HEALTH_BAR_PADDING - arena.enemy.lifeBox->Get<TransformComponent>().position.width;
							}
						}
					}
				}
			}
			else
			{
				printf("Started game\n");
				
				eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "KeyboardInputSystem");
				eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "PhysicsSystem");
				eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "AnimationSystem");
				eventManager->Notify<SystemControlEvent>(SystemControlEvent::ENABLE, "HitBoxSystem");

				displayScreen->RemoveAllComponents();

				connected_with_server = true;
			}
		}
		else if (type == SERVER && event.type == NetworkEvent::RECEIVE){
			/*for (unsigned i = 0; i < event.packets.size(); i++)
			{
				printf("%d: %s\n", i, event.packets[i].data());
			}*/

			for (auto& p : event.packets)
			{
				if (p.empty())
					continue;

				std::string jstr(&p[0], p.size() + 1);
				nlohmann::json j = nlohmann::json::parse(jstr.c_str(), nullptr, false);
				if (j.is_discarded())
				{
					printf("Malformed packet%d\n", __LINE__);
					continue;
				}

				auto head = j.at("head").get<std::string>();
				if (head == "player_keyboard_event") {
					auto pressedKeys = j.at("pressed_keys").get<std::vector<int>>();
					auto heldKeys = j.at("held_keys").get<std::vector<int>>();
					decltype(heldKeys) releasedKeys;

					for (auto& e : pool->GetEntities(1 << GetComponentTypeID<ArenaGameComponent>())) {

						auto& arena = e->Get<ArenaGameComponent>();

						if (j.find("player") != j.end()) {
							std::vector<KeyboardEvent::TriggeredEntity> triggered_entities{
								KeyboardEvent::TriggeredEntity{
									arena.player.ptr,
									pressedKeys,
									releasedKeys,
									heldKeys
								}
							};
							eventManager->Notify<KeyboardEvent>(triggered_entities);
						}
						else if (j.find("enemy") != j.end())
						{
							std::vector<KeyboardEvent::TriggeredEntity> triggered_entities{
								KeyboardEvent::TriggeredEntity{
									arena.enemy.ptr,
									pressedKeys,
									releasedKeys,
									heldKeys
								}
							};
							eventManager->Notify<KeyboardEvent>(triggered_entities);
						}
					}
				}
			}
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
