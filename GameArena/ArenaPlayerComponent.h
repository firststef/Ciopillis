#pragma once
struct ArenaPlayerComponent : IComponent
{
	EntityPtr arena;

	ArenaPlayerComponent(EntityPtr arena): arena(arena) {}
};
