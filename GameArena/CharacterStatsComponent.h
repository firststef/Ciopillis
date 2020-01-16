#pragma once

struct CharacterStatsComponent : IComponent
{
	enum State
	{
		NORMAL,
		STUNNED
	} state = NORMAL;
	
	float base_hp;
	float hp;

	float base_agility;
	float agility;

	CharacterStatsComponent(float base_hp, float base_agility)
		:base_hp(base_hp), hp(base_hp), base_agility(base_agility), agility(base_agility)
	{}
	
};