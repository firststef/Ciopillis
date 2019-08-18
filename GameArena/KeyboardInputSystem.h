#pragma once
class KeyboardInputSystem : public ISystem
{
    void Initialize() override {}

    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<KeyboardInputComponent>()))
        {
            auto& comp = e->Get<KeyboardInputComponent>();

            for (auto k : { KEY_LEFT, KEY_RIGHT,KEY_UP, KEY_DOWN, KEY_X, KEY_Z, KEY_ENTER, KEY_ESCAPE })
            {
                if (std::find(comp.gestures.begin(), comp.gestures.end(), k) != comp.gestures.end())
                    continue;

                if (IsKeyPressed(k))
                    eventManager->Notify<KeyboardEvent>(KeyboardEvent::PRESSED, k);
                else if (IsKeyReleased(k))
                    eventManager->Notify<KeyboardEvent>(KeyboardEvent::RELEASED, k);
                else if (IsKeyDown(k))
                    eventManager->Notify<KeyboardEvent>(KeyboardEvent::HOLD, k);
            }
        }
    }
};