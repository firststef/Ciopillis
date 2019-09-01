#pragma once
#include <chrono>
#include "AnimationEvent.h"

struct AnimationUnit
{
    EntityPtr entity;
    std::string name;

    std::chrono::system_clock::time_point lastIterationTime;
    std::chrono::duration<int, std::ratio<1,1000>> animationSpeed;

    Texture2D oldTexture;
    Rectangle oldSourceRec;

    Texture2D texture;
    Rectangle sourceRec;
    bool mirrorSprite;
    bool reverseOrder;
    unsigned repeats = 1;

    unsigned currentFrame = 0;
    unsigned currentRepeat = 0;
    unsigned numOfFrames;
};

class AnimationSystem : public ISystem
{
    std::vector<AnimationUnit> animationQueue;

    std::vector<AnimationUnit> priorityQueue;

    void SolveAnimation(AnimationUnit& unit)
    {
        auto& sprite = unit.entity->Get<SpriteComponent>();

        sprite.texture = unit.texture;

        unsigned frame = (unit.reverseOrder ? (unit.numOfFrames - 1 - unit.currentFrame) : unit.currentFrame);
        unsigned nextFrame;
        if (unit.repeats == 0)
            nextFrame = (unit.currentFrame + 1) % unit.numOfFrames;
        else
            nextFrame = (unit.currentFrame + 1 == unit.numOfFrames) ? frame : unit.currentFrame + 1;

        if (unit.mirrorSprite)
            sprite.sourceRec = { unit.sourceRec.x + frame * unit.sourceRec.width, 0, - unit.sourceRec.width, unit.sourceRec.height };
        else
            sprite.sourceRec = { unit.sourceRec.x + frame * unit.sourceRec.width, 0, unit.sourceRec.width, unit.sourceRec.height };

        unit.currentFrame = nextFrame;

        if (unit.currentFrame + 1 == unit.numOfFrames)
            unit.currentRepeat++;
    }

public:
    AnimationSystem() : ISystem("AnimationSystem") {}

    void Initialize() override
    {
    }

    void Execute() override
    {
        if (!priorityQueue.empty())
        {
            auto now = std::chrono::system_clock::now();

            if (now - priorityQueue.front().lastIterationTime > priorityQueue.front().animationSpeed)
            {
                if (priorityQueue.front().repeats != 0 && priorityQueue.front().currentRepeat >= priorityQueue.front().repeats)
                {
                    priorityQueue.clear();
                }
                else {
                    SolveAnimation(priorityQueue.front());
                    priorityQueue.front().lastIterationTime = now;
                }
            }
        }
        else if (!animationQueue.empty())
        {
            auto now = std::chrono::system_clock::now();
            for (auto& unit : animationQueue)
            {
                if (now - unit.lastIterationTime > unit.animationSpeed)
                {
                    if (unit.repeats != 0 && unit.currentRepeat >= unit.repeats)
                    {
                        animationQueue.erase(std::remove_if(animationQueue.begin(), animationQueue.end(), [&](AnimationUnit& u) ->bool
                        {
                            return memcmp(&unit, &u, sizeof(unit)) == 0;
                        }), animationQueue.end());
                    }
                    else {
                        SolveAnimation(unit);
                        unit.lastIterationTime = now;
                    }
                }
            }
        }
    }

    void Receive(const AnimationEvent& event)
    {
        AnimationUnit unit;

        unit.entity = event.entity;
        unit.name = event.name;

        unit.lastIterationTime += std::chrono::duration<int, std::ratio<1,1000>>(event.timePerFrame);

        auto& sprite = unit.entity->Get<SpriteComponent>();
        unit.oldTexture = sprite.texture;
        unit.oldSourceRec = sprite.sourceRec;
        unit.reverseOrder = event.reverseOrder;

        unit.texture = event.texture;
        unit.sourceRec = event.sourceRec;

        unit.numOfFrames = event.numOfFrames;
        unit.mirrorSprite = event.mirrorSprite;
        unit.repeats = event.repeats;

        unit.animationSpeed = std::chrono::duration<int, std::ratio<1,1000>>(event.timePerFrame);

        //TODO: aici ar trebui refacut si implementat un graf pentru arborele de animatii

        if (event.othersType == AnimationEvent::OVERRIDE_OTHERS)
        {
            auto it = std::find_if(animationQueue.begin(), animationQueue.end(), 
                [&](const AnimationUnit& u)->bool {return u.entity == event.entity && u.name == event.name; });

            if (it == animationQueue.end())
            {
                animationQueue.clear();

                animationQueue.push_back(unit);
            }
            else {
                if (event.sameType == AnimationEvent::CONTINUE_IF_EXISTENT_SAME)
                {
                    //do nothing
                }
                else if (event.sameType == AnimationEvent::WAIT_SAME)
                {
                    //TODO: wait e mai complex
                }
            }
        }
        else if (event.othersType == AnimationEvent::WAIT_OTHERS)
        {
            //TODO: wait e mai complex
        }
        else if (event.othersType == AnimationEvent::SOLVE_THIS_FIRST)
        {
            priorityQueue.push_back(unit);
        }
    }
};