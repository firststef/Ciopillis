#pragma once
#include "Entity.h"
#include "TransformComponent.h" 

class Pool
{
    std::vector<EntityPtr> entities;

public:
    void Refresh()
    {
       /* entities.erase(std::remove_if(std::begin(entities), std::end(entities),
            [](const EntityPtr &mEntity)
        {
            return !(mEntity->IsActive());
        }),
            std::end(entities));*/
    }

    EntityPtr AddEntity()
    {
        EntityPtr e (new Entity());
        entities.push_back(std::move(e));

        return entities.back();
    }

    std::vector<EntityPtr> GetEntities()
    {
        return std::vector<EntityPtr>(entities.begin(), entities.end());
    }

    std::vector<EntityPtr> GetEntities(const ComponentBitset& bitset)
    {
        std::vector<EntityPtr> group;
        for (auto& ptr : entities)
        {
            if (ptr->Has(bitset))
            {
                group.push_back(ptr);
            }
        }

        return group;
    }

    std::vector<EntityPtr> GetEntities(bool(*func)(EntityPtr ptr))
    {
        std::vector<EntityPtr> group;
        for (auto& ptr : entities)
        {
            if (func(ptr))
            {
                group.push_back(ptr);
            }
        }

        return group;
    }

    EntityPtr GetEntity(const ComponentBitset& bitset)
    {
        for (auto& ptr : entities)
        {
            if (ptr->Has(bitset))
            {
                return ptr;
            }
        }

        return nullptr;
    }

    EntityPtr GetEntity(bool(*func)(EntityPtr ptr))
    {
        for (auto& ptr : entities)
        {
            if (func(ptr))
            {
                return ptr;
            }
        }

        return nullptr;
    }
};
