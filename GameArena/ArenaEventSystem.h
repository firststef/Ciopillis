#pragma once
#include "KeyboardEvent.h"
#include "ArenaPlayerEvent.h"

class ArenaEventSystem : public ISystem
{
public:
    ArenaEventSystem() : ISystem("ArenaEventSystem") {}

    void Initialize() override {}
	
    void Execute() override {}

    void Receive (const KeyboardEvent& event)
    {
        //Action
        if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_X) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_X, Vector2{0,0});
        }
        else if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_Z) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_Z, Vector2{ 0,0 });
        }

        //Move
        const float x = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_RIGHT) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_LEFT) != event.pressedKeys.end()));
        const float y = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_DOWN) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_UP) != event.pressedKeys.end()));

        const float hX = ((std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_RIGHT) != event.heldKeys.end())
            - (std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_LEFT) != event.heldKeys.end()));
        const float hY = ((std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_DOWN) != event.heldKeys.end())
            - (std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_UP) != event.heldKeys.end()));

        Vector2 axes{ x + hX, y + hY };

        eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::MOVE, axes);
    }
};
