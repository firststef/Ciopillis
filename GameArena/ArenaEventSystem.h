#pragma once
#include "KeyboardEvent.h"
#include "ArenaPlayerEvent.h"
#include "ArenaPlayerComponent.h"

class ArenaEventSystem : public ISystem
{
public:
    ArenaEventSystem() : ISystem("ArenaEventSystem") {}

    void Initialize() override {}
	
    void Execute() override {}

    void Receive (const KeyboardEvent& event)
    {
		for (auto& te : event.triggered_entities) {

			if(! te.entity->Has<ArenaPlayerComponent>())
				continue;
			
			auto& apc = te.entity->Get<ArenaPlayerComponent>();

			//Action
			if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_X) != te.pressedKeys.end())
			{
				eventManager->Notify<ArenaPlayerEvent>(apc.arena, ArenaPlayerEvent::ATTACK_X, Vector2{ 0,0 });
			}
			else if (std::find(te.pressedKeys.begin(), te.pressedKeys.end(), KEY_Z) != te.pressedKeys.end())
			{
				eventManager->Notify<ArenaPlayerEvent>(apc.arena, ArenaPlayerEvent::ATTACK_Z, Vector2{ 0,0 });
			}

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

			eventManager->Notify<ArenaPlayerEvent>(apc.arena, ArenaPlayerEvent::MOVE, axes);

			break;
		}
    }
};
