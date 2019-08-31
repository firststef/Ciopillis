#pragma once
#include <raylib.h>

struct AnimationEvent : IEvent
{
    EntityPtr entity;
    std::string name;

    Texture2D texture;
    Rectangle sourceRec;

    unsigned numOfFrames;
    unsigned repeats;
    const unsigned timePerFrame;
    //eventual un void* pentru context, pe care il va distruge in destructor sau nu

    enum CollideWithOthersBehaviour
    {
        WAIT_OTHERS,
        OVERRIDE_OTHERS,
        SOLVE_THIS_FIRST
    } othersType;

    enum CollideWithSameBehaviour
    {
        WAIT_SAME,
        RESTART_SAME,
        CONTINUE_IF_EXISTENT_SAME
    } sameType;

    AnimationEvent(
        EntityPtr entity,
        std::string name,
        CollideWithOthersBehaviour othersType,
        CollideWithSameBehaviour sameType,
        Texture2D texture,
        Rectangle sourceRec,
        unsigned numOfFrames,
        unsigned repeats,
        unsigned timePerFrame
    )
    :   entity(entity),
        name(std::move(name)),
        texture(texture),
        sourceRec(sourceRec),
        numOfFrames(numOfFrames),
        repeats(repeats),
        timePerFrame(timePerFrame),
        othersType(othersType),
        sameType(sameType)
    {
    }
};