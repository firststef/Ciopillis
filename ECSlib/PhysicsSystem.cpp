#include "PhysicsSystem.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include "Helpers.h"

void PhysicsSystem::Initialize() 
{
#ifdef WIN32
    WIN_InitPhysics();
    WIN_SetPhysicsGravity(0, 0);
#else
    InitPhysics();
    SetPhysicsGravity(0, 0);
#endif
}

void PhysicsSystem::Execute() 
{
#ifdef WIN32
    WIN_RunPhysicsStep();
#else
    RunPhysicsStep();
#endif

    for (auto& e : pool->GetEntities(1 << GetComponentTypeID<PhysicsComponent>() | 1 << GetComponentTypeID<TransformComponent>()))
    {
        auto& transComp = e->Get<TransformComponent>();
        auto& phyComp = e->Get<PhysicsComponent>();
        transComp.position = Rectangle{ phyComp.body->position.x - transComp.position.width/2, phyComp.body->position.y - transComp.position.height / 2, transComp.position.width, transComp.position.height };
		transComp.rotation = radiansToDegrees(phyComp.body->orient);
    }
}

void PhysicsSystem::Destroy() 
{
#ifdef WIN32
    WIN_ClosePhysics();
#else
    ClosePhysics();
#endif
}