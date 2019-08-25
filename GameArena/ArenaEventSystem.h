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
        if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_X) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_X, Vector2{0,0});
        }
        else if (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_Y) != event.pressedKeys.end())
        {
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::ATTACK_Y, Vector2{ 0,0 });
        }

        //TODO: aici trebuie verificat daca jucatorul tine apasat pe hold, daca a trecut timpul de cooldown

        const float x = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_RIGHT) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_LEFT) != event.pressedKeys.end()))
            * VELOCITY;
        const float y = ((std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_DOWN) != event.pressedKeys.end())
            - (std::find(event.pressedKeys.begin(), event.pressedKeys.end(), KEY_UP) != event.pressedKeys.end()))
            * VELOCITY;
        if (x != 0 || y != 0)
            eventManager->Notify<ArenaPlayerEvent>(ArenaPlayerEvent::MOVE, Vector2{ x,y });
    }
};
