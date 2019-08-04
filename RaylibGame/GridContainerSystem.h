#pragma once
#include "raylib.h"
#include <algorithm>
#include "System.h"
#include "Components.h"

class GridContainerSystem : public ISystem
{
public:

    void Initialize() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<GridContainerComponent>() | 1 << GetTypeID<TransformComponent>()))
        {
            auto& grid = e->Get<GridContainerComponent>();
            Update(e);
            grid.needsUpdate = false;
        }
    }

    void Execute() override
    {
        for (auto& e : pool->GetEntities(1 << GetTypeID<GridContainerComponent>() | 1 << GetTypeID<TransformComponent>()))
        {
            auto& grid = e->Get<GridContainerComponent>();
            if (grid.needsUpdate)
            {
                Update(e);
                grid.needsUpdate = false;
            }
        }
    }

    void CreateFrame(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();
        const auto pos = e->Get<TransformComponent>();

        const auto totalHeight = pos.position.height - grid.marginUp - grid.marginDown - grid.spaceBetween * (static_cast<float>(grid.numOfLines) - 1.0f);
        const auto totalWidth = pos.position.width - grid.marginLeft - grid.marginRight - grid.spaceBetween * (static_cast<float>(grid.numOfColumns) - 1.0f);

        const auto width = totalWidth / static_cast<float>(grid.numOfColumns);
        const auto height = totalHeight / static_cast<float>(grid.numOfLines);

        for (int lin = 0; lin < grid.numOfLines; lin++)
            for (int col = 0; col < grid.numOfColumns; col++)
            {//CAUTION: turning corner position into center positions
                float y = pos.position.y + grid.marginUp + grid.spaceBetween * static_cast<float>(lin) + height * static_cast<float>(lin) + height / 2;
                float x = pos.position.x + grid.marginLeft + grid.spaceBetween * static_cast<float>(col) + width * static_cast<float>(col) + width / 2;

                const Rectangle aux{ x,y,width,height };
                grid.positionTable[grid.numOfLines * lin + col] = aux;
            }
    }
    int GetFreeFrameIdx(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();

        int idx = 0;
        for (auto& el : grid.items)
        {
            if (el == nullptr)
            {
                return idx;
            }
            ++idx;
        }

        if (grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES) {
            return idx;
        }

        return -1;
    }
    void RecountFrameCells(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();
        
        if (grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES)
        {
            grid.numOfColumns = 0;
            grid.numOfLines = 0;

            if (grid.numberOfContainedElements == 0)
                return;

            grid.numOfLines = 1;
            if (grid.numberOfContainedElements <= grid.maxNumOfColumns)
            {
                grid.numOfColumns = grid.numberOfContainedElements;
                return;
            }

            grid.numOfColumns = grid.maxNumOfColumns;

            while(grid.numOfColumns*grid.numOfLines < grid.numberOfContainedElements)
            {
                ++grid.numOfLines;
            }
        }
    }
    void PlaceItemsInFrame(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();

        int idx = 0;
        for (auto& obj : grid.items) {

            if (obj == nullptr)
                continue;

            auto& pos = obj->Get<TransformComponent>().position;

            auto getPos = grid.positionTable[idx];

            if (grid.stretchEnabled)
            {
                getPos.x -= getPos.width / 2;
                getPos.y -= getPos.height / 2;
            }
            else
            {
                getPos.width = pos.width;
                getPos.height = pos.height;
                getPos.x -= getPos.width / 2;
                getPos.y -= getPos.height / 2;
            }

            pos = getPos;

            ++idx;
        }
    }
    bool AddItem(EntityPtr grid, EntityPtr item)
    {
        auto& cont = grid->Get<GridContainerComponent>();

        auto idx = GetFreeFrameIdx(grid);

        if (idx == cont.items.size())//dynamic add
            cont.items.push_back(nullptr);
        cont.items[idx] = item;

        ++cont.numberOfContainedElements;

        RecountFrameCells(grid);

        cont.needsUpdate = true;

        return true;
    }
    void ReleaseItem(EntityPtr e, EntityPtr item)
    {
        auto& grid = e->Get<GridContainerComponent>();

        auto it = std::find(grid.items.begin(), grid.items.end(), item);
        if (it == grid.items.end())
            return;

        int idx = std::distance(grid.items.begin(), it);

        grid.items[idx] = nullptr;
        grid.numberOfContainedElements--;

        if (grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES)
        {
            std::remove(grid.items.begin() + idx, grid.items.begin() + idx + 1, nullptr);
        }

        RecountFrameCells(e);

        grid.needsUpdate = true;
    }
    void ReinitFrame(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();

        if (grid.items.size() != grid.numOfColumns * grid.numOfLines)
            grid.items.resize(grid.numOfColumns * grid.numOfLines);

        grid.positionTable.clear();
        grid.positionTable.resize(grid.numOfColumns * grid.numOfLines);
    }
    void Update(EntityPtr e)
    {
        ReinitFrame(e);
        CreateFrame(e);
        PlaceItemsInFrame(e);
    }
};