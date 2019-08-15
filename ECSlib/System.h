#pragma once
#include "Pool.h"
#include "SystemControlEvent.h"

class TextureManager;
class EventManager;
struct IEvent;
class SystemManager;

class ISystem
{
protected:
    Pool* pool = nullptr;
    TextureManager* textureManager = nullptr;
    SystemManager* systemManager = nullptr;
    EventManager* eventManager = nullptr;
public:
    bool enabled = true;
    std::string name;

    ISystem(std::string name) : name(name) {}

    virtual void SetDependencies(Pool* pool, TextureManager* textureManager, SystemManager* systemManager, EventManager* eventManager)
    {
        this->pool = pool;
        this->textureManager = textureManager;
        this->systemManager = systemManager;
        this->eventManager = eventManager;
    }
    virtual void Initialize() {}
    virtual void Execute() {}

    void Receive(const SystemControlEvent& event)
    {
        if (event.systemName == this->name && event.controlAction == SystemControlEvent::DISABLE)
        {
            enabled = false;
        }
        else if (event.systemName == this->name && event.controlAction == SystemControlEvent::ENABLE)
        {
            enabled = true;
        }
    }
};

class SystemManager : public ISystem
{
public:
    SystemManager() :ISystem(std::string("SystemManager")) {}

    std::vector<std::shared_ptr<ISystem>> systems;

    void Initialize() override
    {
        for (const auto& ptr : systems )
        {
            if (ptr->enabled)
                ptr->Initialize();
        }
    }

    void Execute() override
    {//TODO: Implementarea multithreaded va merge doar daca pun la subscribe si sistemul care va fi apelat ca sa il opreasca cu enabled=false din rulare 
        for (const auto& ptr : systems)
        {
            if (ptr->enabled)
                ptr->Execute();
        }
    }

    void AddSystem(std::shared_ptr<ISystem> ptr)
    {
        systems.push_back(std::dynamic_pointer_cast<ISystem>(ptr));

        ptr->SetDependencies(pool, textureManager,this, eventManager);
    }

    template<typename T>
    T* Get() const
    {
        for (const auto& ptr : systems)
        {
            if (std::dynamic_pointer_cast<T, ISystem>(std::make_shared<ISystem>(*(ptr.get())))!= nullptr)
                return std::dynamic_pointer_cast<T, ISystem>(std::make_shared<ISystem>(*(ptr.get()))).get();
        }
        return nullptr;
    }
};