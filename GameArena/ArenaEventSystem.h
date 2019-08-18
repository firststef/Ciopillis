#pragma once

class ArenaEventSystem : ISystem
{
    EntityPtr entity;

    void Initialize() override {}
    void Execute() override {}

    void Receive (const KeyboardEvent& event)
    {
    }
};