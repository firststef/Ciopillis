#pragma once
#include "KeyboardEvent.h"
#include "ArenaPlayerEvent.h"

class ArenaEventSystem : public ISystem
{
public:

    Vector2 lastAxes = { 0,0 };

    ArenaEventSystem() : ISystem("ArenaEventSystem") {}

    void Initialize() override {}
    void Execute() override {}

    void Receive (const KeyboardEvent& event)
    {
        //Action
        //TODO: aici trebuie verificat daca jucatorul tine apasat pe hold, daca a trecut timpul de cooldown
        if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_X) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_X, Vector2{0,0});
        }
        else if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_Y) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_Y, Vector2{ 0,0 });
        }

        //Move
        const float x = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_RIGHT) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_LEFT) != event.pressedKeys.end()))
            * VELOCITY;
        const float y = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_DOWN) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_UP) != event.pressedKeys.end()))
            * VELOCITY;

        const float hX = ((std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_RIGHT) != event.heldKeys.end())
            - (std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_LEFT) != event.heldKeys.end()))
            * VELOCITY;
        const float hY = ((std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_DOWN) != event.heldKeys.end())
            - (std::find(event.heldKeys.begin(), event.heldKeys.end(), KEY_UP) != event.heldKeys.end()))
            * VELOCITY;

        Vector2 axes{ x + hX, y + hY };

        if ( ! ( axes == lastAxes && lastAxes == Vector2{0,0}))
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::MOVE, axes);

        lastAxes = axes;
    }
};
