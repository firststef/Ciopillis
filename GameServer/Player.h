#pragma once
#include <string>
#include "Card.h"

class Player
{
public:
    std::string                             name;
    Element                                 currentElement = None;

    int                                     points = 0;

    CardContainer                           base;

    CardContainer                           hand;
    CardContainer                           draw;
    CardContainer                           discard;

    unsigned                                maxCards = 5;
    bool                                    cardPlayed = false;
    unsigned                                selectedCardIndex = -1;

    Player(std::string name, CardContainer base) :
        name(std::move(name)), base(base)
    {
        
    }
};
