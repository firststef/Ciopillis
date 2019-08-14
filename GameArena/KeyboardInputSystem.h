#pragma once

class KeyboardInputSystem : public ISystem
{
    bool inputEnabled = true;

    EntityPtr entity;

    KeyboardInputSystem(EntityPtr entity)
    {
        if (entity->Has(1 << GetTypeID<TransformComponent>()))
            this->entity = entity;
    }

    void Initialize() override {}

    void Execute() override
    {
        if (IsKeyDown(KEY_LEFT))
        {

        }
    }
};