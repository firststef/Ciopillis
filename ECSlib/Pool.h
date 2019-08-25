#pragma once
#include "Entity.h"

class Pool
{
    std::vector<EntityPtr> entities;

public:

    EntityPtr AddEntity()
    {
        EntityPtr e (new Entity());
        entities.push_back(std::move(e));

        return entities.back();
    }

    void RemoveEntity(EntityPtr e)
    {
        entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
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

    std::vector<EntityPtr> GetEntities(bool(*func)(EntityPtr ptr, void* context), void* context)
    {
        std::vector<EntityPtr> group;
        for (auto& ptr : entities)
        {
            if (func(ptr, context))
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
