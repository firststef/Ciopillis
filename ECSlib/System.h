#pragma once
#include "Pool.h"
#include "SystemControlEvent.h"

class ISystem;

using SystemID = std::size_t;
constexpr std::size_t maxSystems = 32;

inline SystemID GetSystemID()
{
	static SystemID lastID = 0;

	return lastID++;
}

template <typename T>
SystemID GetSystemTypeID()
{
	static_assert(std::is_base_of<ISystem, T>::value, "T is not derived from system");

	static SystemID typeID = GetSystemID();

	return typeID;
}

class TextureManager;
class EventManager;
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
    virtual void Destroy() {}

    void Receive(const SystemControlEvent& event)
    {
        if (event.systemName == this->name)
        {
			if (event.controlAction == SystemControlEvent::DISABLE)
			{
				enabled = false;
			}
			else if (event.controlAction == SystemControlEvent::ENABLE)
			{
				enabled = true;
			}
        }
    }
};

using SystemPtr = std::shared_ptr<ISystem>;

class SystemManager
{
	Pool* pool = nullptr;
	TextureManager* textureManager = nullptr;
	EventManager* eventManager = nullptr;
public:
	SystemManager(Pool* pool, TextureManager* textureManager, EventManager* eventManager)
		:pool(pool), textureManager(textureManager), eventManager(eventManager)
	{
	}
	
    std::array<SystemPtr, maxSystems> systems {nullptr};

    void Initialize()
    {
        for (const auto& ptr : systems )
        {
            if (ptr.get() && ptr->enabled)//TODO: systems might rather need Initialize(enabled) or Execute(Enabled) to run in "stealth" mode.
                ptr->Initialize();
        }
    }

    void Execute()
    {
        for (const auto& ptr : systems)
        {
            if (ptr.get() && ptr->enabled)
                ptr->Execute();
        }
    }

    void Destroy()
    {
        for (const auto& ptr : systems)
        {
			if (ptr.get())
				ptr->Destroy();
        }
    }

	template<typename T, typename... TArgs>
    T& AddSystem(TArgs&&... mArgs)
    {
		T* ptr(new T(std::forward<TArgs>(mArgs)...));
		SystemPtr sPtr{ ptr };
        systems[GetSystemTypeID<T>()] = sPtr;

        ptr->SetDependencies(pool, textureManager,this, eventManager);

		return *ptr;
    }

    template<typename T>
    T* Get() const
    {
		auto ptr = std::dynamic_pointer_cast<T, ISystem>(std::make_shared<ISystem>(*(systems[GetSystemTypeID<T>()].get())));
    	
		if (ptr != nullptr)
			return ptr;
        
        return nullptr;
    }
};