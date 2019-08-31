#pragma once
#include "KeyboardInputComponent.h"

class KeyboardInputSystem : public ISystem
{
    std::vector<int> trackedKeys = { KEY_LEFT, KEY_RIGHT,KEY_UP, KEY_DOWN, KEY_X, KEY_Z, KEY_ENTER, KEY_ESCAPE };

public:
    KeyboardInputSystem() : ISystem("KeyboardInputSystem") {}

    void Initialize() override {}
     
    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<KeyboardInputComponent>()))
        {
            auto& comp = e->Get<KeyboardInputComponent>();

            std::vector<int> pressedKeys;
            std::vector<int> releasedKeys;
            std::vector<int> heldKeys;

            for (auto k : trackedKeys)
            {
                if (std::find(comp.gestures.begin(), comp.gestures.end(), k) == comp.gestures.end())
                    continue;

                if (IsKeyPressed(k))
                    pressedKeys.push_back(k);
                else if (IsKeyReleased(k))
                    releasedKeys.push_back(k);
                else if (IsKeyDown(k))
                    heldKeys.push_back(k);
            }

            if (!pressedKeys.empty() || !releasedKeys.empty() || !heldKeys.empty())
                eventManager->Notify<KeyboardEvent>(pressedKeys, releasedKeys, heldKeys);//TODO: de mutat keyb in lib
        }
    }
};
