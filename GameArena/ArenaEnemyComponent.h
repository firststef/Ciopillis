#pragma once
struct ArenaEnemyComponent : IComponent
{
	EntityPtr arena;

	ArenaEnemyComponent(EntityPtr arena) : arena(arena) {}
};
