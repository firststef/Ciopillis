#pragma once
#include <utility>

struct HitBoxTriggerInfo
{
    Shape::Identifier identifier1;
    Shape::Identifier identifier2;
};

struct HitBoxEvent : IEvent
{
    std::vector<HitBoxTriggerInfo> allTriggerInfos;

    HitBoxEvent(std::vector<HitBoxTriggerInfo> allTriggerInfos)
        :allTriggerInfos(std::move(allTriggerInfos))
    {}
};