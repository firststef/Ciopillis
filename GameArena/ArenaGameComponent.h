#pragma once
#include "Component.h"
#include <ctime>

struct ArenaGameComponent : IComponent
{
    enum GameStates
    {
        INIT,
        RUNNING,
        ENDED
    }                               state = INIT;

    std::vector<EntityPtr>          generatedEntities;

    enum CurrentAction
    {
        IDLE,
        MOVE,
        ATTACK_X,
        ATTACK_Z
    };

	//Dash
	enum DashState
	{
		INITIAL,
		NOT_READY,
		READY,
		DASHED
	};

	friend std::ostream& operator<<(std::ostream& out, const DashState value) {
		const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
		switch (value) {
			PROCESS_VAL(INITIAL);
			PROCESS_VAL(NOT_READY);
			PROCESS_VAL(READY);
			PROCESS_VAL(DASHED);
		}
#undef PROCESS_VAL

		return out << s;
	}

	struct ArenaCharacterAttributes
	{
		EntityPtr ptr;
		CurrentAction currentAction = IDLE;
		Vector2 lastAxes = {-1, 1};
		std::shared_ptr<bool> orientation = std::make_shared<bool>();

		//Dash
		DashState dashState = INITIAL;
		time_t dashCounter = time(nullptr);
		float dashOrientation = 0;

		bool blockInput = false;

		EntityPtr target;
		bool targetActive = false;
		time_t lastTargetTime = time(nullptr);

		bool triggerTarget = false;

		EntityPtr lifeBox = nullptr;
		
	} player, enemy;
};
