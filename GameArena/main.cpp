#include "raylib.h"
#include "ECSlib.h"
#include "Constants.h"
#include "ArenaEvent.h"
#include "KeyboardInputSystem.h"
#include "KeyboardEvent.h"
#include "ArenaEventSystem.h"
#include "ArenaGameComponent.h"
#include "ArenaSystem.h"

int main()
{
    //Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle);
    SetTargetFPS(60);

    ECSManager manager;

    auto fighter(manager.pool.AddEntity());

    fighter->Add<TransformComponent>(Rectangle{ 500,500,200,200 });
    fighter->Add<SpriteComponent>(std::string("Fighter"), Color(ORANGE));

    auto arenaSystem = std::make_shared<ArenaSystem>(ArenaSystem());
    auto drawSystem = std::make_shared<DrawSystem>(DrawSystem());
    //auto eventSystem = std::make_shared<EventSystem>(EventSystem(server));
    //auto enemySystem = std::make_shared<EnemySystem>(EnemySystem(server, enemyHand, enemyDraw, enemyDiscard, playZone));

    manager.systemManager.AddSystem(arenaSystem);
    manager.systemManager.AddSystem(drawSystem);
    //manager.systemManager.AddSystem(eventSystem);

    return 1;
}