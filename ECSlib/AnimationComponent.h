#pragma once
#include <chrono>
#include <utility>
#include "Component.h"

struct AnimationUnit
{
    //resources
    std::string name;

    Texture2D texture;
    Rectangle sourceRec;
    unsigned numOfFrames;
    unsigned timePerFrame;
    std::shared_ptr<bool> mirrorSprite;
    std::shared_ptr<bool> reverseOrder;
    unsigned repeats;

    //utils
    unsigned currentFrame;
    unsigned currentRepeat;

    bool started;

    std::chrono::duration<int, std::ratio<1, 1000>> animationSpeed;

    AnimationUnit(
        std::string name,
        Texture2D texture,
        Rectangle sourceRec,
        unsigned numOfFrames,
        const unsigned timePerFrame,
        std::shared_ptr<bool> mirrorSprite,
        std::shared_ptr<bool> reverseOrder,
        unsigned repeats)
        :name(std::move(name)),
        texture(texture),
        sourceRec(sourceRec),
        numOfFrames(numOfFrames),
        timePerFrame(timePerFrame),
        mirrorSprite(mirrorSprite),
        reverseOrder(reverseOrder),
        repeats(repeats)
    {
        currentFrame = 0;
        currentRepeat = 0;
        started = false;
        animationSpeed = std::chrono::duration<int, std::ratio<1, 1000>>(timePerFrame);
    }
};

struct AnimationGraph;

struct AnimationNode
{
    std::shared_ptr<AnimationUnit> animationUnit;
    std::function<void(AnimationGraph&, void*)> animationEventHandlerFunction;
    void* context;

    struct NextAnimationNodeWrapper
    {
        std::shared_ptr<AnimationNode> node;
        std::function<bool(const AnimationNode& node, void* context)> cond;
        void* context;
    };

    std::vector<NextAnimationNodeWrapper> nextNodes;

    explicit AnimationNode(AnimationUnit anim, std::function<void(AnimationGraph&, void*)> animationEventHandlerFunction, void* context)
    : animationUnit(std::make_shared<AnimationUnit>(anim)), 
    animationEventHandlerFunction(std::move(animationEventHandlerFunction)),
    context(context)
    {}

    std::shared_ptr<AnimationNode> Next(std::shared_ptr<AnimationNode> next, std::function<bool(const AnimationNode& node, void* context)> cond, void* context)
    {
        nextNodes.push_back(NextAnimationNodeWrapper{ next, cond, context });

        return next;
    }
};

struct AnimationGraph
{
    std::shared_ptr<AnimationNode> zero;
    std::shared_ptr<AnimationNode> currentNode;

    std::chrono::system_clock::time_point lastIterationTime;

    explicit AnimationGraph(std::shared_ptr<AnimationNode> zero) : zero(zero), currentNode(zero) {}
};

struct AnimationComponent : IComponent
{
    AnimationGraph graph;

    AnimationComponent(AnimationGraph graph) : graph(std::move(graph)) {}
};