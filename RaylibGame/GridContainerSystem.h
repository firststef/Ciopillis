#pragma once
#include "raylib.h"
#include "System.h"
#include "Components.h"
#include <queue>

class HorizontalContainer : public Container
{
public:
    enum AllocateType
    {
        GET_FIRST_AVAILABLE,
        GET_LAST_AVAILABLE
    };

    AllocateType                            allocateType = GET_FIRST_AVAILABLE;

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

    HorizontalContainer(std::string str, int z, Color col, Rectangle pos, ContainerType t, int columns, int lines, float left, float up, float right, float down, float space);

    void                                    InitSize();
    int                                     AssignPos();
    void                                    OverwritePos();

    ~HorizontalContainer();
};
HorizontalContainer::HorizontalContainer(std::string str, int z, Color col, Rectangle pos, ContainerType t, int columns, int lines,
    float left, float up, float right, float down, float space) : Container(std::move(str), z, col, pos, t)
{
    draw = [&]()->void
    {
        if (!overwritePosOn)
        {
            OverwritePos();
            overwritePosOn = true;
        }

        if (type == OVERLAY || type == MATERIAL)
            DrawRectangleRec(position, color);

        if (type == WRAPPER || type == MATERIAL)
            for (auto obj = children.begin(); obj != children.end(); ++obj) {
                obj->GetDraw()();
            }
    };
    color = PURPLE;
    InitSize();
};

void HorizontalContainer::InitSize()
{
    positionTable = (Rectangle*)calloc(numOfColumns*numOfLines, sizeof(Rectangle));

    const auto totalHeight = position.height - marginUp - marginDown - spaceBetween * (static_cast<float>(numOfLines) - 1.0f);
    const auto totalWidth = position.width - marginLeft - marginRight - spaceBetween * (static_cast<float>(numOfColumns) - 1.0f);

    const auto width = totalWidth / static_cast<float>(numOfColumns);
    const auto height = totalHeight / static_cast<float>(numOfLines);

    for (int lin = 0; lin < numOfLines; lin++)
        for (int col = 0; col < numOfColumns; col++)
        {//CAUTION: turning corner position into center positions
            float y = position.y + marginUp + spaceBetween * static_cast<float>(lin) + height * static_cast<float>(lin) + height / 2;
            float x = position.x + marginLeft + spaceBetween * static_cast<float>(col) + width * static_cast<float>(col) + width / 2;

            const Rectangle aux{ x,y,width,height };
            positionTable[numOfLines * lin + col] = aux;
        }

    indexTable = (int*)calloc(numOfColumns*numOfLines, sizeof(int));
    optimizeIndexTable = (int*)calloc(numOfColumns*numOfLines, sizeof(int));
    savedPositionTable = (Rectangle*)calloc(numOfColumns*numOfLines, sizeof(Rectangle));
}
int HorizontalContainer::AssignPos()
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
void HorizontalContainer::OverwritePos()
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
HorizontalContainer::~HorizontalContainer()
{
    free(positionTable);
    free(indexTable);
    free(optimizeIndexTable);
    free(savedPositionTable);
}

class GridContainerSystem : public ISystem
{
public:

    struct EventRequest
    {
        enum RequestType
        {
            NONE
        };

        EntityPtr entity;
        EntityPtr container;
    };

    std::queue<EventRequest> queue;

    void Initialize() override {}

    void Execute() override
        {
        for (auto& e : pool->GetEntities(1 << GetTypeID<GridContainerComponent>()))
            {
            auto& comp = e->Get<GridContainerComponent>();
            if (comp.needsUpdate)
                comp.Update();
            }
        }

private:
    bool CreateFrame(EntityPtr ptr);
    int GetEmptyPos();
    bool 
};