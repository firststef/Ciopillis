#pragma once
#include <raylib.h>

inline Rectangle operator+(Rectangle &a, Rectangle b)
{
    return Rectangle { a.x + b.x,a.y + b.y,a.width + b.width,a.height + b.height };
}

inline Rectangle operator-(Rectangle &a, Rectangle b)
{
    return Rectangle{ a.x - b.x,a.y - b.y,a.width - b.width,a.height - b.height };
}