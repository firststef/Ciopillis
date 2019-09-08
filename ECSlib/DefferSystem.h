#pragma once
#include <chrono>
#include "DefferEvent.h"

struct VoidContextFuncCoverter
{
    VoidContextFuncCoverter(std::function<void(void*)> func, void* context) : func(std::move(func)), context(context) {}
    void operator()() { func(context); }
    std::function<void(void*)> func;
    void* context;
};

struct DelayHolder
{
    std::chrono::system_clock::time_point endPoint;
    VoidContextFuncCoverter f;
};

class DefferSystem : public ISystem
{
    std::vector<DelayHolder> delayVector;

public:

    DefferSystem() : ISystem("DefferSystem") {}

    void Initialize() override {}

    void Execute() override
    {
        auto now = std::chrono::system_clock::now();

        for (auto it = delayVector.begin(); it != delayVector.end();)
        {
            if (now >= it->endPoint)
            {
                it->f();
                it = delayVector.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void Receive(const DefferEvent& event)
    {
        delayVector.push_back(DelayHolder{
                std::chrono::system_clock::now() + std::chrono::duration<int, std::ratio<1, 1000>>(event.delayTime),
                VoidContextFuncCoverter{
                    event.func,
                    event.context
                }
            });
    }
};