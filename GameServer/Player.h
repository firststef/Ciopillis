#pragma once
#include <string>
#include <memory>
#include "Card.h"

class Player
{
public:
    std::string                             name;
    Element                                 currentElement = None;

    int                                     points = 0;

    CardContainer                           base;

    std::shared_ptr<CardContainer>          hand;
    std::shared_ptr<CardContainer>          draw;
    std::shared_ptr<CardContainer>          discard;

    unsigned                                maxCards = 5;
    bool                                    cardPlayed = false;
    unsigned                                selectedCardIndex = -1;

    Player(std::string name, CardContainer base) :
        name(std::move(name)), base(base)
    {
        
    }
};
