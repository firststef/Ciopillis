#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"

void PhysicsSystem::Initialize() 
{
    WIN_InitPhysics();
    WIN_SetPhysicsGravity(0, 0);
}

void PhysicsSystem::Execute() 
{
    WIN_RunPhysicsStep();

    for (auto& e : pool->GetEntities(1 << GetTypeID<PhysicsComponent>() | 1 << GetTypeID<TransformComponent>()))
    {
        auto& transComp = e->Get<TransformComponent>();
        auto& phyComp = e->Get<PhysicsComponent>();
        transComp.position = Rectangle{ phyComp.body->position.x, phyComp.body->position.y, transComp.position.width, transComp.position.height };
    }
}

void PhysicsSystem::Destroy() 
{
    WIN_ClosePhysics();
}