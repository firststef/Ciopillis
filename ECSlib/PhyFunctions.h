#pragma once
PhysicsBody WIN_GetBody(int type, float x, float y, float first, float second, float third);
void WIN_InitPhysics();
void WIN_SetPhysicsGravity(float x, float y);
void WIN_RunPhysicsStep();
void WIN_ClosePhysics();