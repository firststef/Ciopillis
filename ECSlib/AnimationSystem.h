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
    unsigned repeats = 1;

    unsigned currentFrame = 0;
    unsigned currentRepeat = 0;
    unsigned numOfFrames;
};

class AnimationSystem : public ISystem
{
    std::vector<AnimationUnit> animationQueue;

    std::vector<AnimationUnit> priorityQueue;

    void SolveAnimation(std::chrono::system_clock::time_point now, AnimationUnit& unit)
    {
        if (now - unit.lastIterationTime > unit.animationSpeed)
        {
            auto& sprite = unit.entity->Get<SpriteComponent>();

            sprite.texture = unit.texture;
            sprite.sourceRec = { unit.sourceRec.x + unit.currentFrame * unit.sourceRec.width, 0, unit.sourceRec.width, unit.sourceRec.height };

            if (unit.repeats == 0)
                unit.currentFrame = (unit.currentFrame + 1) % unit.numOfFrames;
            else
                unit.currentFrame = (unit.currentFrame + 1 == unit.numOfFrames) ? unit.numOfFrames : unit.currentFrame + 1;

            unit.lastIterationTime = now;

            if (unit.currentFrame == unit.numOfFrames)
                unit.currentRepeat++;
        }
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

            SolveAnimation(now, priorityQueue.front());

            priorityQueue.clear();
        }
        else if (!animationQueue.empty())
        {
            auto now = std::chrono::system_clock::now();
            for (auto& unit : animationQueue)
            {
                if (unit.repeats != 0 && unit.currentRepeat > unit.repeats)
                {
                    animationQueue.erase(std::remove_if(animationQueue.begin(), animationQueue.end(), [&](AnimationUnit& u) ->bool
                    {
                        return memcmp(&unit, &u, sizeof(unit)) == 0;
                    }), animationQueue.end());
                }

                SolveAnimation(now, unit);
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

        unit.texture = event.texture;
        unit.sourceRec = event.sourceRec;

        unit.numOfFrames = event.numOfFrames;
        unit.repeats = event.repeats;

        unit.animationSpeed = std::chrono::duration<int, std::ratio<1,1000>>(event.timePerFrame);

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