#pragma once
#include "ECS.h"

class PositionComponent : public Component
{
private:
    int xPos;
    int yPos;

public:
    int x() { return xPos; }
    int y() { return yPos; }

    void Init() override
    {
        xPos = 0;
        yPos = 0;
    }

    void Update() override
    {

    }

    void setPos(int x, int y)
    {
        xPos = x;
        yPos = y;
    }
};