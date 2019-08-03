#pragma once
#include "Component.h"

struct GridContainerComponent : IComponent
{
    //Engine-related
    bool needsUpdate = true;

    //System-related
    enum AllocateType
    {
        GET_FIRST_AVAILABLE,
        GET_LAST_AVAILABLE
    } allocateType = GET_FIRST_AVAILABLE;

    int                                     numOfColumns = 1;
    int                                     numOfLines = 1;

    float                                   marginLeft = 0;
    float                                   marginUp = 0;
    float                                   marginRight = 0;
    float                                   marginDown = 0;

    float                                   spaceBetween = 0;

    bool                                    stretchEnabled = false;

    Rectangle*                              positionTable = nullptr;
    Rectangle*                              savedPositionTable = nullptr;
    int*                                    indexTable = nullptr;
    int*                                    optimizeIndexTable = nullptr;
    bool                                    overwritePosOn = false;

    GridContainerComponent(int columns, int lines, float left, float up, float right, float down, float space);
    int AssignPos();
};

inline GridContainerComponent::GridContainerComponent(int columns, int lines, float left, float up, float right,
    float down, float space)
{

    InitSize();
}

int GridContainerComponent::AssignPos()
{
    switch (allocateType)
    {
    case GET_LAST_AVAILABLE:
        for (int idx = numOfColumns * numOfLines - 1; idx >= 0; idx--)
            if (optimizeIndexTable[idx] == 0)
            {
                optimizeIndexTable[idx] = 1;
                return idx;
            }
        return -1;
    case GET_FIRST_AVAILABLE:
    default:
        for (int idx = 0; idx < numOfColumns*numOfLines; idx++)
            if (optimizeIndexTable[idx] == 0)
            {
                optimizeIndexTable[idx] = 1;
                return idx;
            }
        return -1;
    }
}
void GridContainerComponent::OverwritePos()
{
    int index = 0;
    for (auto obj = children.begin(); obj != children.end(); ++obj) {
        auto genericObj = static_cast<GameObject*>((*obj).GetPointer());

        savedPositionTable[indexTable[index]] = genericObj->position;
        auto getPos = positionTable[indexTable[index]];

        if (stretchEnabled)
        {
            getPos.x -= getPos.width / 2;
            getPos.y -= getPos.height / 2;
        }
        else
        {
            getPos.width = genericObj->position.width;
            getPos.height = genericObj->position.height;
            getPos.x -= getPos.width / 2;
            getPos.y -= getPos.height / 2;
        }

        genericObj->position = getPos;
        //genericObj->draw();
        index++;
    }
}
