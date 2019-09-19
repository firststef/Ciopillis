#pragma once
struct HitBoxComponent : IComponent
{
    ShapeContainer cont;

    HitBoxComponent(ShapeContainer cont)
        :cont(std::move(cont))
    {}
};