#pragma once

class ArenaSystem : public ISystem
{

    void OnInit(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        auto fadedBackground(pool->AddEntity());
        fadedBackground->Add<TransformComponent>(Rectangle{ 0,0, SCREEN_WIDTH, SCREEN_HEIGHT });
        fadedBackground->Add<SpriteComponent>(std::string("FadedBackground"), Texture2D(), Fade(BLACK, 0.6f));
        arena.generatedEntities.push_back(fadedBackground);

        auto fighter(pool->AddEntity());
        fighter->Add<TransformComponent>(Rectangle{ 500,500,200,200 });
        fighter->Add<SpriteComponent>(std::string("Fighter"), Texture2D(), Color(ORANGE));
        fighter->Add<KeyboardInputComponent>();
        arena.generatedEntities.push_back(fighter);

        arena.state = ArenaGameComponent::RUNNING;
    }

    void OnRunning(EntityPtr e)
    {

    }

    void OnEnd(EntityPtr e)
    {
        auto& arena = e->Get<ArenaGameComponent>();

        arena.generatedEntities.clear();
    }

public:
    ArenaSystem() : ISystem(std::string("ArenaSystem")) {}

    void Initialize() override
    {   
    }

    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<ArenaGameComponent>()))
        {
            auto arena = e->Get<ArenaGameComponent>();

            switch (arena.state) {
            case ArenaGameComponent::INIT:
                OnInit(e);
            case ArenaGameComponent::RUNNING:
                OnRunning(e);
            case ArenaGameComponent::ENDED:
                OnEnd(e);
            default:
                break;
            }
        }
    }
};