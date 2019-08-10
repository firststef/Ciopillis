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
                grid.positionTable[(grid.numOfLines - 1)* lin + col] = aux;
            }
    }
    int GetFreeFrameIdx(EntityPtr e)
    {
        auto& grid = e->Get<GridContainerComponent>();

        unsigned idx = 0;
        for (auto& el : grid.items)
        {
            if (el == nullptr)
            {
                break;
            }
            ++idx;
        }

        if (grid.itemSetMode == GridContainerComponent::FIXED_GET_FIRST_AVAILABLE) {
            if (idx == grid.items.size())
                return -1;
            return idx;
        }
        if (grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES) {
            return grid.items.size();
        }
        //IF SETINPLACE - TREBuie verificat in ce locatie trebuie pusa cartea
        if (grid.itemSetMode == GridContainerComponent::INFINITE_STACK) {
            return 0;
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

        if (grid.itemSetMode == GridContainerComponent::FIXED_GET_FIRST_AVAILABLE || grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES) {
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
        else if (grid.itemSetMode == GridContainerComponent::FIXED_SET_IN_PLACE)
        {
            int idx = 0;
            for (auto& obj : grid.items) {

                if (obj == nullptr)
                {
                    ++idx;
                    continue;
                }

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
        else if (grid.itemSetMode == GridContainerComponent::INFINITE_STACK) {
            for (auto& obj : grid.items) {

                if (obj == nullptr)
                    continue;

                auto& pos = obj->Get<TransformComponent>().position;

                auto getPos = grid.positionTable[0];

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
            }
        }
    }
    bool AddItem(EntityPtr grid, EntityPtr item)
    {
        auto& cont = grid->Get<GridContainerComponent>();

        auto idx = GetFreeFrameIdx(grid);

        if (cont.itemSetMode == GridContainerComponent::FIXED_GET_FIRST_AVAILABLE || cont.itemSetMode == GridContainerComponent::FIXED_SET_IN_PLACE)
            if (idx == -1)
                return false;

        if (cont.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES || cont.itemSetMode == GridContainerComponent::INFINITE_STACK)
            cont.items.push_back(nullptr);
        
        cont.items[idx] = item;
        item->Add<GridContainerChildComponent>(grid, idx);

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
        item->Remove<GridContainerChildComponent>();

        grid.numberOfContainedElements--;

        if (grid.itemSetMode == GridContainerComponent::DYNAMIC_ERASE_SPACES || grid.itemSetMode == GridContainerComponent::FIXED_GET_FIRST_AVAILABLE)
        {
            grid.items.erase(grid.items.begin() + idx, grid.items.begin() + idx + 1);//erase

            if (grid.itemSetMode == GridContainerComponent::FIXED_GET_FIRST_AVAILABLE)
            {
                grid.items.push_back(nullptr);
            }
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
        RecountFrameCells(e);
        ReinitFrame(e);
        CreateFrame(e);
        PlaceItemsInFrame(e);
    }

    void Receive(const GridAddRemoveEvent& event)
    {
        if (event.type == GridAddRemoveEvent::ADD)
        {
            AddItem(event.parent, event.entity);
        }
        else if(event.type == GridAddRemoveEvent::REMOVE)
        {
            ReleaseItem(event.parent, event.entity);
        }
    }
};