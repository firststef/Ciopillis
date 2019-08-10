#pragma once
#include "Card.h"

class CardGenerator
{
public:
    CardContainer container;
    
    CardGenerator() = default;
    CardGenerator(std::string path);

    CardContainer GetDatabase(std::string path);
};

