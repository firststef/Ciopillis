#pragma once
#include <string>
#include "Card.h"

class Player
{
public:
    std::string                                 name;

    int                                     points = 0;

    CardContainer                           hand;
    CardContainer                           draw;
    CardContainer                           discard;

    Player(std::string name) :
        name(std::move(name))
    {
        
    }
};
